#include <iostream>
#include <SDL.h>
#include <math.h>
#include <vector>
#include <ctime>
#include <SDL_ttf.h>
#include <cstring> // memset
#include <algorithm> //sort
#include "raycasting_game.h"

/* sky width has to be a multiple of 360*2 (angle)*/
#define SKY_WIDTH 720
#define SKY_HEIGHT 576

using namespace std;

void cast_rays(Player& p, Map& m, Textures& t, SDL_Renderer* renderer);
void handle_input (SDL_Event& e, Player& p);
void draw_sky(SDL_Renderer* renderer, Player& p, Textures& t);

int Debugger::fps_count = 0;
long Debugger::time_stamp = 0;
long Debugger::time_stamp_old = 0;
long Debugger::ms_per_frame = 50;
long Debugger::current_frame = 0;
TTF_Font* Debugger::font = NULL;
double Data::cos_vals[3600000];
double Data::sin_vals[3600000];
double Data::tan_vals[3600000];
int Player::plane_y = 200;
int Player::height = 32;

int main(int argc, char** argv) {
	
	
	Data::preload_math_vars();
	
	Debugger db;
	TTF_Init();
	Debugger::font = TTF_OpenFont( "res/txt.otf", 12 );
	
	SDL_Window *window = NULL;
	SDL_Surface *surface = NULL;
	SDL_Renderer *renderer = NULL;
	
	
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
		cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
		return 1;
	}
	
	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode(0, &dm);
	int screen_multiplier = dm.w % Data::render_size_x > 0 ? (dm.w / Data::render_size_x) - 1 : dm.w / Data::render_size_x;
	while (screen_multiplier * Data::render_size_y > dm.h) screen_multiplier--;
	screen_multiplier--;
	screen_multiplier = screen_multiplier < 1 ? 1 : screen_multiplier;
	
	window = SDL_CreateWindow( "intersection test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Data::render_size_x * screen_multiplier, Data::render_size_y * screen_multiplier, /*SDL_WINDOW_FULLSCREEN*/ SDL_WINDOW_SHOWN);
	if( window == NULL ) {
		cout <<  "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
		return 1;
	}
	
	
	surface = SDL_GetWindowSurface(window);
	SDL_Event e;
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
	Textures t(renderer);
	
	
	Player p;
	p.dist_player_to_plane = Player::plane_x / 2 / tan(30 * M_PI / 180);
	Map m;
	
	for (int i = 0; i < m.field_num_x; i++) {
		m.map[i][0].wall = true;
		m.map[i][0].size = 64;
		m.map[0][i].wall = true;
		m.map[0][i].size = 64;
		
		if (i == m.field_num_x - 1) {
			for (int n = 0; n < m.field_num_x; n++) {
				m.map[i][n].wall = true;
				m.map[i][n].size = 64;
				m.map[n][i].wall = true;
				m.map[n][i].size = 64;
			}
		}
	}
	
	m.map[10][10].wall = true;
	m.map[11][11].wall = true;
	m.map[10][10].size = 25;
	m.map[11][11].size = 25;
	
	m.map[13][10].wall = true;
	m.map[13][10].size = 40;
	
	m.map[2][2].size = 10;
	m.map[2][3].size = 20;
	m.map[2][4].size = 30;
	m.map[2][5].size = 40;
	m.map[2][6].size = 50;
	
	
	SDL_ShowCursor(SDL_FALSE);
	SDL_WarpMouseInWindow(window, Data::render_size_x/2, Data::render_size_y/2);
	int game_delay = 20;
	while (true) {
		
		handle_input(e, p);
		p.move(m);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		draw_sky(renderer, p, t);
		
		t.determine_current_weapon(renderer, p);
		cast_rays(p, m, t, renderer);
		t.draw_current_weapon(renderer);
		
		Debugger::time_stamp_old = Debugger::time_stamp;
		Debugger::time_stamp = SDL_GetTicks();
		int ms_per_frame = 1000.0 * (Debugger::time_stamp - Debugger::time_stamp_old) / CLOCKS_PER_SEC;
		Debugger::ms_per_frame = ms_per_frame;
		if (1000/ms_per_frame < 60 && game_delay > 14) {
			game_delay-=2;
		} else if (1000/ms_per_frame > 60) {
			game_delay+=2;
		}
		//cout << 1000/Debugger::ms_per_frame << " " << Debugger::ms_per_frame <<endl;
		Debugger::draw_info(renderer, 1000/ms_per_frame, ms_per_frame, game_delay);
		SDL_RenderSetScale(renderer, screen_multiplier, screen_multiplier);
		SDL_RenderPresent(renderer);
		SDL_WarpMouseInWindow(window, Data::render_size_x/2, Data::render_size_y/2);
		//Debugger::current_frame++;
		SDL_Delay(game_delay);
	}
	
}


void draw_sky(SDL_Renderer* renderer, Player& p, Textures& t) {
	SDL_Rect src_rect_1;
	SDL_Rect src_rect_2;
	
	SDL_Rect dest_rect_1;
	SDL_Rect dest_rect_2;
	
	if (p.angle * 4 + Data::render_size_x > SKY_WIDTH) {
		
		src_rect_1.x = int(p.angle*4) % SKY_WIDTH;
		src_rect_1.y = 0;
		src_rect_1.w = Data::render_size_x - (int(p.angle * 4 + Data::render_size_x) % SKY_WIDTH);
		src_rect_1.h = SKY_HEIGHT;
		
		//cout << p.plane_y << endl;
		
		dest_rect_1.x = 0;
		dest_rect_1.y = p.plane_y/2 - SKY_HEIGHT + 200;
		dest_rect_1.w = src_rect_1.w;
		dest_rect_1.h = SKY_HEIGHT;
		
		
		src_rect_2.x = 0;
		src_rect_2.y = 0;
		src_rect_2.w = Data::render_size_x - src_rect_1.w;
		src_rect_2.h = SKY_HEIGHT;
		
		
		dest_rect_2.x = dest_rect_1.w;
		dest_rect_2.y = p.plane_y/2 - SKY_HEIGHT + 200;
		dest_rect_2.w = Data::render_size_x - dest_rect_1.w;
		dest_rect_2.h = SKY_HEIGHT;
		
		SDL_RenderCopy(renderer, t.sky, &src_rect_1, &dest_rect_1);
		SDL_RenderCopy(renderer, t.sky, &src_rect_2, &dest_rect_2);
		
		
	} else {
		
		src_rect_1.x = (int)p.angle*4 % SKY_WIDTH;
		src_rect_1.y = 0;
		src_rect_1.w = Data::render_size_x;
		src_rect_1.h = SKY_HEIGHT;
		
		dest_rect_1.x = 0;
		dest_rect_1.y = p.plane_y/2 - SKY_HEIGHT + 200;
		dest_rect_1.w = Data::render_size_x;
		dest_rect_1.h = SKY_HEIGHT;
		
		SDL_RenderCopy(renderer, t.sky, &src_rect_1, &dest_rect_1);
		
	}
	
}

/*
void cast_rays(Player& p, Map& m, Textures& t, SDL_Renderer* renderer) {
		
	for (int n = plane_pixel.y; n < Player::plane_y + (Data::render_size_y - Player::plane_y); n++) {
		
		
		/*pixels that are behind weapon wont be calculated*//*
		if (t.current_weapon_texture != NULL) {
			SDL_Point point = {i, n};
			if (SDL_EnclosePoints(&point, 1, &t.current_weapon_texture->pos, NULL)) {
				/*wegen dieser Zeile hab ich die Nacht durchgemacht*//*
				if (!t.current_weapon_texture->alpha_values[ ((n - t.current_weapon_texture->pos.y) * t.current_weapon_texture->pos.w) + (plane_pixel.x - t.current_weapon_texture->pos.x) ] ) { 
					plane_pixel.y++;
					continue;
				}
			}
		}
		
	}
}*/

void cast_rays(Player& p, Map& m, Textures& t, SDL_Renderer* renderer) {
	
	double current_angle = p.angle - Player::field_of_view/2;
	current_angle = current_angle < 0 ? current_angle + 360 : current_angle;
	
	Uint32 floor_pixels[Data::render_size_x * Data::render_size_y];
	memset(floor_pixels, 0, Data::render_size_x * Data::render_size_y * sizeof(Uint32));
	
	for (int x = 0; x < Player::plane_x; x++) {
		
		vector<raydata_t> intersections;
		
		//find first horizontal grid
		SDL_Rect first_grid_horizontal;
		if (current_angle > 180) {
			first_grid_horizontal.y = (p.pos_y / Field::width) * Field::width + Field::width;
		} else {
			first_grid_horizontal.y = (p.pos_y / Field::width) * Field::width - 1;
		}
		
		first_grid_horizontal.x = p.pos_x + (p.pos_y - first_grid_horizontal.y) / Data::get_tan_val( 180 - current_angle);
		
		
		//calculate horizontal stepsize
		SDL_Rect grid_stepsize_horizontal;
		if (current_angle > 180) {
			grid_stepsize_horizontal.y = Field::width;
			grid_stepsize_horizontal.x = Field::width / Data::get_tan_val(current_angle - 180);
		} else {
			grid_stepsize_horizontal.y = -Field::width;
			grid_stepsize_horizontal.x = Field::width / Data::get_tan_val(180 - current_angle);
		}
		
		//find first vertical grid
		SDL_Rect first_grid_vertical;
		if (current_angle < 90 || current_angle > 270) {
			first_grid_vertical.x = (p.pos_x / Field::width) * Field::width - 1;
			first_grid_vertical.y = p.pos_y + (p.pos_x - first_grid_vertical.x) * Data::get_tan_val(180 - current_angle);
		} else {
			first_grid_vertical.x = (p.pos_x / Field::width) * Field::width + Field::width;
			first_grid_vertical.y = p.pos_y + (p.pos_x - first_grid_vertical.x) * Data::get_tan_val(180 - current_angle);
		}
		
		
		//calculate vertical stepsize
		SDL_Rect grid_stepsize_vertical;
		if (current_angle < 90 || current_angle > 270) {
			grid_stepsize_vertical.x = -Field::width;
			grid_stepsize_vertical.y = -Field::width * Data::get_tan_val(current_angle - 180);
		} else {
			grid_stepsize_vertical.x = Field::width;
			grid_stepsize_vertical.y = Field::width * Data::get_tan_val(current_angle - 180);
		}
		
		
		//calculate grid steps
		SDL_Rect current_grid_horizontal = first_grid_horizontal;
		SDL_Rect current_grid_vertical = first_grid_vertical;
		for (int i = 0; i < Player::view_depth; i++) {
			
			if (current_grid_horizontal.x >= 0 && current_grid_horizontal.x / Field::width < m.field_num_x && current_grid_horizontal.y >= 0 && current_grid_horizontal.y / Field::height < m.field_num_y) {
				
				SDL_Rect field_pos;
				field_pos.x = current_grid_horizontal.x / Field::width;
				field_pos.y = current_grid_horizontal.y / Field::width;
				
				int distance = sqrt(pow(p.pos_x - current_grid_horizontal.x, 2) + pow(p.pos_y - current_grid_horizontal.y, 2));
				distance *= Data::get_cos_val(p.angle - current_angle);
				distance = distance == 0 ? 1 : distance; //division by 0 forbidden
				
				raydata_t rd = {field_pos, current_grid_horizontal.x, distance, &m.map[field_pos.x][field_pos.y]};
				
				intersections.push_back(rd);
			}
			
			if (current_grid_vertical.x >= 0 && current_grid_vertical.x / Field::width < m.field_num_x && current_grid_vertical.y >= 0 && current_grid_vertical.y / Field::height < m.field_num_y) {
					
					SDL_Rect field_pos;
					field_pos.x = current_grid_vertical.x / Field::width;
					field_pos.y = current_grid_vertical.y / Field::width;
					
					int distance = sqrt(pow(p.pos_x - current_grid_vertical.x, 2) + pow(p.pos_y - current_grid_vertical.y, 2));
					distance *= Data::get_cos_val(p.angle - current_angle);
					distance = distance == 0 ? 1 : distance; //division by 0 forbidden
					
					raydata_t rd = {field_pos, current_grid_vertical.y, distance, &m.map[field_pos.x][field_pos.y]};
					
					intersections.push_back(rd);
				}
			
			current_grid_horizontal.x += grid_stepsize_horizontal.x;
			current_grid_horizontal.y += grid_stepsize_horizontal.y;
			
			current_grid_vertical.x += grid_stepsize_vertical.x;
			current_grid_vertical.y += grid_stepsize_vertical.y;
		}
		
		// operator < used by default, shortest distance chosen
		std::sort( intersections.begin(), intersections.end() );
		
		vector<raydata_t>::iterator end = std::unique( intersections.begin(), intersections.end() );
		
		
		int last_field_height = 0;
		int last_y_wallpos = Data::render_size_y;
		int last_wallfloor_height = Data::render_size_y;
		for (vector<raydata_t>::iterator it = intersections.begin(); it != end; it++) {
			
			
			SDL_Rect r_dest;
			SDL_Rect r_src;
			
			r_dest.h = (double)it->field->size/it->distance * p.dist_player_to_plane + 1;
			r_dest.y = (double)p.height / ((double)it->distance / p.dist_player_to_plane) + p.plane_y/2 - r_dest.h;
			r_dest.w = 1;
			r_dest.x = x;
			
			r_src.w = 1;
			r_src.h = Field::height; //hier texture eigene height angeben
			r_src.y = 0;
			r_src.x = it->x_pos % Field::width;
			
			// draw wall
			bool wall_found = false;
			if (it->field->size > 0 && r_dest.y < last_y_wallpos) {
				
				
				if (r_dest.y + r_dest.h > last_y_wallpos) {
					double percentage = (double)(last_y_wallpos - r_dest.y) / r_dest.h;
					r_dest.h = last_y_wallpos - r_dest.y;
					r_src.h *= percentage;
				}
				
				SDL_RenderCopy(renderer, t.forest_wall, &r_src, &r_dest);
				
				wall_found = true;
			}
			
			//draw floor
			if (it->field->size != last_field_height) {
				
				
				int floor_draw_length = last_y_wallpos;
				
				SDL_Rect plane_pixel;
				if (wall_found) {
					
					plane_pixel.y = r_dest.y + r_dest.h;
					
				} else {
					int height = (double)last_field_height/it->distance * p.dist_player_to_plane + 1;
					int y = (double)p.height / ((double)it->distance / p.dist_player_to_plane) + p.plane_y/2 - height;
					if (y > last_y_wallpos) continue;
					plane_pixel.y = y;
					last_y_wallpos = y;
					
				}
				
				
				plane_pixel.w = 1;
				plane_pixel.h = 1;
				plane_pixel.x = x;
				
				//cout << r_dest.y + r_dest.h << " " << last_y_wallpos << endl;
				//SDL_RenderDrawLine(renderer, x, plane_pixel.y, x, floor_draw_length);
				
				
				for (int y = plane_pixel.y; y < floor_draw_length; y++) {
					
					int straight_distance_to_floor = ((double)(p.height - last_field_height)/ (y - Player::plane_y/2)) * p.dist_player_to_plane;
					int cos_angle = p.angle - current_angle;
					cos_angle = cos_angle < 0 ? -cos_angle : cos_angle;
					int actual_distance_to_floor = (double)straight_distance_to_floor / Data::get_cos_val(cos_angle);
					
					SDL_Rect texture_pixel;
					texture_pixel.w = 1;
					texture_pixel.h = 1;
					
					texture_pixel.x = (int)(p.pos_x - actual_distance_to_floor * Data::get_cos_val(current_angle)) % Field::width;
					texture_pixel.y = (int)(p.pos_y - actual_distance_to_floor * Data::get_sin_val(current_angle)) % Field::height;
					
					if (plane_pixel.y * Data::render_size_x + plane_pixel.x >= 0 && plane_pixel.y * Data::render_size_x + plane_pixel.x <= Data::render_size_x * Data::render_size_y) {
						floor_pixels[plane_pixel.y * Data::render_size_x + plane_pixel.x] = t.forest_floor_texdata.color_values[texture_pixel.y * 64 + texture_pixel.x];
					}
					
					plane_pixel.y++;
				}
				
			}
			
			last_y_wallpos = wall_found ? r_dest.y : last_y_wallpos;
			last_field_height = it->field->size;
			last_wallfloor_height = wall_found ? r_dest.y + r_dest.h : last_wallfloor_height;
		}
		
		/*
		
		
		//draw floor
		SDL_Rect plane_pixel;
		plane_pixel.w = 1;
		plane_pixel.h = 1;
		plane_pixel.x = x;
		plane_pixel.y = r_dest.y + r_dest.h;
		
		for (int n = plane_pixel.y; n < last_y_pos; n++) {
			
			int straight_distance_to_floor = ((double)Player::height / (n - Player::plane_y/2)) * p.dist_player_to_plane;
			int cos_angle = p.angle - current_angle;
			cos_angle = cos_angle < 0 ? -cos_angle : cos_angle;
			int actual_distance_to_floor = (double)straight_distance_to_floor / Data::get_cos_val(cos_angle);
			
			SDL_Rect texture_pixel;
			texture_pixel.w = 1;
			texture_pixel.h = 1;
			
			texture_pixel.x = (int)(p.pos_x - actual_distance_to_floor * Data::get_cos_val(current_angle)) % Field::width;
			texture_pixel.y = (int)(p.pos_y - actual_distance_to_floor * Data::get_sin_val(current_angle)) % Field::height;
			
			
			if (plane_pixel.y * Data::render_size_x + plane_pixel.x >= 0 && plane_pixel.y * Data::render_size_x + plane_pixel.x <= Data::render_size_x * Data::render_size_y) {
				floor_pixels[plane_pixel.y * Data::render_size_x + plane_pixel.x] = t.forest_floor_texdata.color_values[texture_pixel.y * 64 + texture_pixel.x];
			}
			
			plane_pixel.y++;
			
		}
		
		last_y_pos = r_dest.y;
		*/
		
		current_angle += (double)Player::field_of_view/Player::plane_x;
		current_angle = current_angle > 360 ? current_angle - 360 : current_angle;
	}
	
	SDL_Surface *temp = SDL_CreateRGBSurface(0, Data::render_size_x, Data::render_size_y, 32, 0, 0, 0, 0);
	memcpy(temp->pixels, floor_pixels, sizeof(floor_pixels));
	SDL_SetColorKey(temp, SDL_TRUE, 0);
	SDL_Texture *floor_texture = SDL_CreateTextureFromSurface(renderer, temp);
	SDL_RenderCopy(renderer, floor_texture, NULL, NULL);
	SDL_DestroyTexture(floor_texture);
	SDL_FreeSurface(temp);
}

void handle_input (SDL_Event& e, Player& p) {
	while( SDL_PollEvent( &e ) != 0 ) {

		if( e.type == SDL_KEYDOWN ) {
		
			switch( e.key.keysym.sym ) {
				case SDLK_LEFT:
					p.angle++;
					p.angle = p.angle >= 360 ? p.angle - 360 : p.angle;
					break;
				case SDLK_RIGHT:
					p.angle--;
					p.angle = p.angle <= 0 ? 360 + p.angle : p.angle;
					break;
				case SDLK_UP:
					Player::height+=10;
					Player::plane_y += 10;
					cout << Player::height << endl;
					break;
				case SDLK_DOWN:
					Player::height-=10;
					Player::plane_y -= 10;
					cout << Player::height << endl;
					break;
				case SDLK_a:
					p.key_a = true;
					break;
				case SDLK_d:
					p.key_d = true;
					break;
				case SDLK_w: 
					p.key_w = true;
					break;
				case SDLK_s: 
					p.key_s = true;
					break;
				case SDLK_ESCAPE:
					cout << Debugger::ms_per_frame << " " << 1000/Debugger::ms_per_frame << endl;
					exit(0);
					break;
				default:
					break;
			}
			//cout << p.angle <<  endl;
			
		} else if (e.type == SDL_KEYUP) {
			switch( e.key.keysym.sym ) {
				case SDLK_a:
					p.key_a = false;
					break;
				case SDLK_d:
					p.key_d = false;
					break;
				case SDLK_w: 
					p.key_w = false;
					break;
				case SDLK_s: 
					p.key_s = false;
					break;
				case SDLK_r: 
					p.reloading = true;
					break;
				default:
					break;
			}
			
		} else if (e.type == SDL_MOUSEMOTION) {
			
			int x, y;
			SDL_GetMouseState( &x, &y );
			if (x < p.plane_x/2) {
				
				p.last_direction = 0;
				p.angle -= (p.plane_x/2 - x)/ (100./Debugger::ms_per_frame);
				p.angle = p.angle <= 0 ? 360 + p.angle : p.angle;
				
			} else if (x > p.plane_x/2){
				
				p.last_direction = 1;
				p.angle += (x - p.plane_x/2)/ (100./Debugger::ms_per_frame);
				p.angle = p.angle >= 360 ? p.angle - 360 : p.angle;
				
			}
			
			if (y < Data::render_size_y/2) {
			
				if (Player::plane_y < 700) Player::plane_y += (Data::render_size_y/2 - y)/ (20./Debugger::ms_per_frame);
				
			} else if (y > Data::render_size_y/2) {
				if (Player::plane_y > -120) Player::plane_y -= (y - Data::render_size_y/2)/ (20./Debugger::ms_per_frame);
				//Player::plane_y = Player::plane_y < 0 ? 0 : Player::plane_y;
			}
			
			//cout << Player::plane_y << endl;
			
		} else if (e.type == SDL_MOUSEBUTTONDOWN) {
			
			switch (e.button.button) {
				case SDL_BUTTON_LEFT:
					p.shooting = true;
					break;
			}
			
		}
		//cout << p.angle << endl;
		
	}
}










