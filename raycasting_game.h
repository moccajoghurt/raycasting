#ifndef RAYCASTING_GAME_H
#define RAYCASTING_GAME_H

#include <sstream>
#include <vector>

using namespace std;


class Data {
public:
	
	static double cos_vals[];
	static double sin_vals[];
	static double tan_vals[] ;
	
	static const int render_size_x = 320; //320;
	static const int render_size_y = 200; //200;
	
	static bool shader_activated;
	
	static void preload_math_vars() {
		long absolute = 0;
		for (double d = 0; d < 360; d+= 0.0001) {
			cos_vals[absolute] = cos(d * M_PI / 180);
			sin_vals[absolute] = sin(d * M_PI / 180);
			if (d * M_PI / 180 == 90) {tan_vals[absolute] = tan(90.1);}
			else if (d * M_PI / 180 == 270) {tan_vals[absolute] = tan(270.1);}
			else {tan_vals[absolute] = tan(d * M_PI / 180);}
			absolute++;
		}
		
	}
	
	static double get_cos_val(double val) {
		double buf = val < 0 ? -val : val;
		//double d = cos_vals[(long)(buf*10000)];
		double d = cos(buf * M_PI / 180);
		return d;
	}
	
	static double get_sin_val(double val) {
		double buf = val < 0 ? -val : val;
		//double d = sin_vals[(long)(buf*10000)];
		double d = sin(buf * M_PI / 180);
		return d;
	}
	
	static double get_tan_val(double val) {
		double buf = val < 0 ? -val : val;
		//double d = tan_vals[(long)(buf*10000)];
		double d = tan(buf * M_PI / 180);
		if (buf * M_PI / 180 == 90) {d = tan(90.1 * M_PI / 180);}
		else if (buf * M_PI / 180 == 270) {d = tan(270.1 * M_PI / 180);}
		if (val < 0) return -d;
		else return d;
	}
	
};


class Field {
public:
	int size;
	static const int width = 64;
	static const int height = 64;
	Field() {
		size = 1;
	}
};

typedef struct raydata_t {
	
	bool operator==(const raydata_t& rd) const {
		return field_pos.x == rd.field_pos.x && field_pos.y == rd.field_pos.y;
	}
	
	bool operator<(const raydata_t& rd) const {
		return distance < rd.distance;
	}
	SDL_Rect field_pos;
	double x_pos;
	double distance;
	Field* field;
	bool horizontal_wall;
	
}raydata_t;

class Map {
public:
	static const int field_num_x = 30;
	static const int field_num_y = 30;
	Field map[field_num_x][field_num_y];
};


class Player {
public:
	static int height;
	static const int plane_x = Data::render_size_x;
	static int plane_y;
	static const int field_of_view = 60;
	static const int view_depth = 30;
	static const int view_distance = 10000;
	
	static int jump_counter;
	
	
	static bool jumping;
	bool shooting;
	bool reloading;
	bool key_w;
	bool key_a;
	bool key_s;
	bool key_d;
	int dist_player_to_plane;
	double pos_x;
	double pos_y;
	double angle;
	int last_direction;
	
	Player() {
		jump_counter = 0;
		jumping = false;
		shooting = false;
		reloading = false;
		key_w = false;
		key_a = false;
		key_s = false;
		key_d = false;
		angle = 230;
		pos_x = Field::width * 4;
		pos_y = Field::height * 4;
		last_direction = 0;
	}
	void handle_jumping (void) {
		
		if (jumping == true) {
			
			if (jump_counter < 5) {
				Player::height += 10;
				
			} else if (jump_counter < 10) {
				Player::height -= 10;
				
			} else {
				jumping = false;
				jump_counter = -1;
			}
			
			jump_counter++;
		}
		
	}
	void move(Map& m);
	bool check_wall_collision(double x, double y, Map& m);
};


class Debugger {
public:

	static int fps_count;
	static long time_stamp;
	static long time_stamp_old;
	static long ms_per_frame;
	static long current_frame;
	static TTF_Font* font; 
	
	static void draw_info(SDL_Renderer* renderer, int fps_value, int ms_per_frame_value, int delay_value) {
		SDL_Color txt_color = {255, 0, 0};
		SDL_Surface* fps;
		stringstream ss;
		ss << "fps: " << fps_value;
		fps = TTF_RenderText_Solid( Debugger::font, ss.str().c_str() , txt_color );
		SDL_Rect fps_pos;
		fps_pos.x = Player::plane_x - 100;
		fps_pos.y = 10;
		fps_pos.w = fps->w;
		fps_pos.h = fps->h;
		SDL_Texture* tex_fps = SDL_CreateTextureFromSurface(renderer, fps);
		SDL_RenderCopy(renderer, tex_fps, NULL, &fps_pos);
		
		SDL_Surface* ms_per_frame;
		ss.str("");
		ss << "ms per frame: " << ms_per_frame_value;
		ms_per_frame = TTF_RenderText_Solid( Debugger::font, ss.str().c_str() , txt_color );
		SDL_Rect mpf_pos;
		mpf_pos.x = Player::plane_x - 100;
		mpf_pos.y = 25;
		mpf_pos.w = ms_per_frame->w;
		mpf_pos.h = ms_per_frame->h;
		SDL_Texture* tex_mpf = SDL_CreateTextureFromSurface(renderer, ms_per_frame);
		SDL_RenderCopy(renderer, tex_mpf, NULL, &mpf_pos);
		
		
		SDL_Surface* delay_frame;
		ss.str("");
		ss << "game delay: " << delay_value;
		delay_frame = TTF_RenderText_Solid( Debugger::font, ss.str().c_str() , txt_color );
		SDL_Rect delay_pos;
		delay_pos.x = Player::plane_x - 100;
		delay_pos.y = 40;
		delay_pos.w = delay_frame->w;
		delay_pos.h = delay_frame->h;
		SDL_Texture* tex_delay = SDL_CreateTextureFromSurface(renderer, delay_frame);
		SDL_RenderCopy(renderer, tex_delay, NULL, &delay_pos);
		
		SDL_DestroyTexture(tex_fps);
		SDL_DestroyTexture(tex_mpf);
		SDL_DestroyTexture(tex_delay);
		SDL_FreeSurface(fps);
		SDL_FreeSurface(ms_per_frame);
		SDL_FreeSurface(delay_frame);
	}
	
	static void draw_map_overview(Player& p, SDL_Renderer* renderer) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		//draw grid
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		for (int i = 0; i < Map::field_num_x; i++) {
			for (int n = 0; n < Map::field_num_y; n++) {
				
				SDL_RenderDrawLine(renderer, i * p.plane_x / Map::field_num_x, 0, i * p.plane_x / Map::field_num_x, p.plane_y);
				SDL_RenderDrawLine(renderer, 0, n * p.plane_y / Map::field_num_y, p.plane_x, n * p.plane_y / Map::field_num_y);
			}
		}
		
		// draw player
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
		SDL_Rect r;
		r.x = ((double)p.pos_x  / (Map::field_num_x * Field::width)) * p.plane_x ;
		r.y = ((double)p.pos_y  / (Map::field_num_y * Field::width)) * p.plane_y ;
		r.w = 2;
		r.h = 2;
		SDL_RenderFillRect(renderer, &r);
		
		SDL_RenderPresent(renderer);
	}
	
	static void draw_absolute_pos(Player& p, SDL_Renderer* renderer) {
		
		//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		//SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		for (int i = 0; i < Map::field_num_x; i++) {
			for (int n = 0; n < Map::field_num_y; n++) {
				
				SDL_RenderDrawLine(renderer, i * Field::width, n * Field::width, n * Field::width + Field::width, n * Field::width);
				SDL_RenderDrawLine(renderer, n * Field::width, i * Field::width, n * Field::width, i * Field::width + Field::width);
			}
		}
		
		SDL_Rect r;
		r.x = p.pos_x;
		r.y = p.pos_y;
		r.w = 8;
		r.h = 8;
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
		SDL_RenderFillRect(renderer, &r);
		
	}
	
};

void Player::move(Map& m) {
	SDL_Rect new_pos;
	double new_pos_x;
	double new_pos_y;
	if (key_w) {
		new_pos_x = this->pos_x - (Debugger::ms_per_frame/4) * cos((this->angle) * M_PI / 180);
		new_pos_y = this->pos_y - (Debugger::ms_per_frame/4) * sin((this->angle) * M_PI / 180);
		if (!this->check_wall_collision(new_pos_x, new_pos_y, m)) {
			this->pos_x = new_pos_x;
			this->pos_y = new_pos_y;
		}
	}
	if (key_a) {
		new_pos_x = this->pos_x + (Debugger::ms_per_frame/4) * cos((270 - this->angle) * M_PI / 180);
		new_pos_y = this->pos_y + (Debugger::ms_per_frame/4) * sin((90 - this->angle) * M_PI / 180);
		if (!this->check_wall_collision(new_pos_x, new_pos_y, m)) {
			this->pos_x = new_pos_x;
			this->pos_y = new_pos_y;
		}
	}
	if (key_s) {
		new_pos_x = this->pos_x + (Debugger::ms_per_frame/4) * cos((this->angle) * M_PI / 180);
		new_pos_y = this->pos_y + (Debugger::ms_per_frame/4) * sin((this->angle) * M_PI / 180);
		if (!this->check_wall_collision(new_pos_x, new_pos_y, m)) {
			this->pos_x = new_pos_x;
			this->pos_y = new_pos_y;
		}
	}
	if (key_d) {
		new_pos_x = this->pos_x + (Debugger::ms_per_frame/4) * cos((90 - this->angle) * M_PI / 180);
		new_pos_y = this->pos_y + (Debugger::ms_per_frame/4) * sin((270 - this->angle) * M_PI / 180);
		if (!this->check_wall_collision(new_pos_x, new_pos_y, m)) {
			this->pos_x = new_pos_x;
			this->pos_y = new_pos_y;
		}
	}
	
}

bool Player::check_wall_collision(double x, double y, Map& m) {
	
	if (x >= 0 && x/Field::width < Map::field_num_x && y >= 0 && y/Field::height < Map::field_num_y) {
	
		if (m.map[(int)x/Field::width][(int)y/Field::height].size - Player::height > -20) {
			
			return true ;
		}
		else {
			if (Player::jumping == false)
				Player::height = m.map[(int)x/Field::width][(int)y/Field::height].size + 40;
			//Player::jumping = false;
			//Player::jump_counter = 0;
			return false;
		}
		
	} else {
		return true;
	}
}

class Animations {
public:
	static const int pistol_reload_timer = 2;
	static const int pistol_shoot_timer = 1;
	
};

typedef struct texture_data_t {
	
	SDL_Rect pos;
	SDL_Texture* texture;
	vector<bool> alpha_values;
	vector<Uint32> color_values;
	Uint32 format;
	
}texture_data_t;

class Textures {
public:
	
	texture_data_t* current_weapon_texture;
	
	SDL_Surface* forest_wall_src;
	SDL_Texture* forest_wall;
	
	SDL_Surface* forest_floor_src;
	SDL_Texture* forest_floor;
	texture_data_t forest_floor_texdata;
	
	SDL_Surface* sky_src;
	SDL_Texture* sky;
	
	SDL_Surface* pistol_src;
	vector<texture_data_t> pistol_idle;
	vector<texture_data_t> pistol_reload;
	vector<texture_data_t> pistol_shoot;
	vector <bool> pistol_alpha_pixel;
	int texture_timer;
	int texture_index;
	
	
	Textures(SDL_Renderer* renderer) {
		
		current_weapon_texture = NULL;
		
		texture_timer = 0;
		texture_index = 0;
		
		forest_wall_src = SDL_LoadBMP("res/forest_wall.bmp");
		forest_wall = SDL_CreateTextureFromSurface(renderer, forest_wall_src);
		
		forest_floor_src = SDL_LoadBMP("res/forest_floor.bmp");
		forest_floor = SDL_CreateTextureFromSurface(renderer, forest_floor_src);
		forest_floor_texdata = load_texture(0, 0, 64, 64, 0, renderer, forest_floor_src);
		
		sky_src = SDL_LoadBMP("res/sky.bmp");
		sky = SDL_CreateTextureFromSurface(renderer, sky_src);
		
		
		/* pistol */
		pistol_src = SDL_LoadBMP("res/pistol.bmp");
		//idle
		pistol_idle.push_back(load_texture(122, 153, 56, 69, Data::render_size_x/2 + 20, renderer, pistol_src));
		//reload
		pistol_reload.push_back(load_texture(18, 13, 61, 80, Data::render_size_x/2 + 20, renderer, pistol_src));
		pistol_reload.push_back(load_texture(90, 10, 148, 98, Data::render_size_x/2, renderer, pistol_src));
		pistol_reload.push_back(load_texture(251, 11, 142, 98, Data::render_size_x/2, renderer, pistol_src));
		pistol_reload.push_back(load_texture(407, 11, 135, 99, Data::render_size_x/2, renderer, pistol_src));
		pistol_reload.push_back(load_texture(19, 123, 70, 99, Data::render_size_x/2 + 20, renderer, pistol_src));
		//shoot
		pistol_shoot.push_back(load_texture(209, 141, 70, 80, Data::render_size_x/2 + 20, renderer, pistol_src));
		pistol_shoot.push_back(load_texture(302, 140, 72, 84, Data::render_size_x/2 + 20, renderer, pistol_src));
		pistol_shoot.push_back(load_texture(391, 123, 58, 98, Data::render_size_x/2 + 20, renderer, pistol_src));
		pistol_shoot.push_back(load_texture(473, 126, 80, 77, Data::render_size_x/2 + 20, renderer, pistol_src));
		pistol_shoot.push_back(load_texture(602, 142, 97, 68, Data::render_size_x/2 + 20, renderer, pistol_src));
	}
	
	void determine_current_weapon (SDL_Renderer* renderer, Player& p) {
		
		if (p.shooting) {
			texture_handling(renderer, pistol_shoot, Animations::pistol_shoot_timer, p.shooting);
			
		} else if (p.reloading) {
			texture_handling(renderer, pistol_reload, Animations::pistol_reload_timer, p.reloading);
			
		} else {
			current_weapon_texture = &pistol_idle[0];
		}
	}
	
	void draw_current_weapon(SDL_Renderer* renderer) {
		SDL_RenderCopy(renderer, current_weapon_texture->texture, NULL, &current_weapon_texture->pos);
	}
	
	void texture_handling(SDL_Renderer* renderer, vector<texture_data_t>& vec, int max_timer, bool& condition) {
		
		if (texture_timer == max_timer) {
			texture_index++; 
			texture_timer = 0;
		}
		if (texture_index == vec.size() - 1) {
			current_weapon_texture = &vec[texture_index];
			texture_timer = 0;
			texture_index = 0;
			condition = false;
			
		} else {
			current_weapon_texture = &vec[texture_index];
			texture_timer++;
		}
		
	
	}
	
	texture_data_t load_texture(int x, int y, int w, int h, int tex_pos_x, SDL_Renderer* renderer, SDL_Surface* s) {
		//idle
		SDL_Rect pos;
		pos.x = x;
		pos.y = y;
		pos.w = w;
		pos.h = h;
		SDL_Surface* temp = SDL_CreateRGBSurface(0, pos.w, pos.h, 32, 0, 0, 0, 0);
		SDL_BlitSurface(s, &pos, temp, NULL);
		SDL_SetColorKey(temp, SDL_TRUE, 0x00FFFF);
		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, temp);
		SDL_Rect tex_pos;
		tex_pos.w = pos.w;
		tex_pos.h = pos.h;
		tex_pos.x = tex_pos_x;
		tex_pos.y = Data::render_size_y - h;
		vector<bool> alpha_vals;
		vector<Uint32> color_vals;
		Uint32 format = load_color_info(renderer, temp, tex, alpha_vals, color_vals);
		texture_data_t td = {tex_pos, tex, alpha_vals, color_vals, format};
		return td;
		
	}
	
	/* unused, no performance boost */
	Uint32 load_color_info(SDL_Renderer* renderer, SDL_Surface* surface, SDL_Texture* tex, vector<bool>& alpha_vals, vector<Uint32>& color_vals) {
		Uint32 format;
		int access, w, h;
		SDL_QueryTexture(tex, &format, &access, &w, &h);
		//SDL_SetRenderTarget(renderer, tex);
		
		int pixel_count = 0;
		for (int i = 0; i < w; i++) {
			for (int n = 0; n < h; n++) {
				
				SDL_Color c;
				
				SDL_PixelFormat* fmt;
				Uint32 temp, pixel;
				Uint8 red, green, blue, alpha;
				
				fmt = surface->format;
				SDL_LockSurface(surface);
				
				pixel = *((Uint32*)surface->pixels + pixel_count);
				
				//red
				temp = pixel & fmt->Rmask;
				temp = temp >> fmt->Rshift;
				temp = temp << fmt->Rloss;
				red = (Uint8)temp;
				
				//green
				temp = pixel & fmt->Gmask;
				temp = temp >> fmt->Gshift;
				temp = temp << fmt->Gloss;
				green = (Uint8)temp;
				
				//blue
				temp = pixel & fmt->Bmask;
				temp = temp >> fmt->Bshift;
				temp = temp << fmt->Bloss;
				blue = (Uint8)temp;
				
				//alpha
				temp = pixel & fmt->Amask;
				temp = temp >> fmt->Ashift;
				temp = temp << fmt->Aloss;
				alpha = (Uint8)temp;
				
				c.r = red;
				c.g = green;
				c.b = blue;
				c.a = alpha;
				
				if (c.r == 0 && c.g == 255 && c.b == 255) { alpha_vals.push_back(true);}
				else { alpha_vals.push_back(false); }
				
				color_vals.push_back(pixel);
				
				pixel_count++;
			}
		}
		return format;
	}
	
	/* unused, no performance boost */
	SDL_Color get_texture_color(vector<SDL_Color>& vec, SDL_Rect* rect) {
		SDL_Color c = vec[rect->y * Field::width + rect->x];
		return c;
	}
	
	void shade_pixel(Uint32* pixel, int distance) {
		
		Uint32 temp;
		Uint8 red, green, blue, alpha;
		
		//red
		red = *pixel >> 24;
		
		//green
		temp = *pixel << 8;
		green = temp >> 24;
		
		//blue
		temp = *pixel << 16;
		blue = temp >> 24;
		
		//alpha
		temp = *pixel << 24;
		alpha = temp >> 24;
		
		
		double old_range = Player::view_distance - 1;
		double new_range = 3; 
		double dist_weight = (((distance - 1) * new_range) / old_range) + 1;
		
		//cout <<  NewValue << endl;
		
		
		//double distance_weight = distance / 100 < 1 ? 1 : distance / 500;
		
		red /= dist_weight;
		green /= dist_weight;
		blue /= dist_weight;
		alpha /= dist_weight;
	
		
		//red
		temp = red;
		temp <<= 24;
		*pixel = temp;
		
		//green
		temp = green;
		temp <<= 16;
		*pixel += temp;
		
		//blue
		temp = blue;
		temp <<= 8;
		*pixel += temp;
		
		//alpha
		temp = alpha;
		*pixel += temp;
		
	}
	
};



#endif
