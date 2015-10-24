//Copyright (C) 2014-2015  Corwin Hansen
#include "main.h"//main header


int main(int argc, char *argv[]){
	renderer = NULL;//set values to null
	window = NULL;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){//start SDL and get any error if it dosen't.
		printf("Could not load SDL: %s\n", SDL_GetError());//print out error
		exit(EXIT_FAILURE);//exit
	}




	// load support for PNG image format
	int flags = IMG_INIT_PNG;//flag for PNG image loading
	int initted = IMG_Init(flags);//get initialised flags
	if ((initted&flags) != flags) {//if it caused error
		printf("Could not initialised png support: %s\n", IMG_GetError());//print error message
		exit(EXIT_FAILURE);//exit
	}


	//initialise ttf
	if (TTF_Init() == -1) {//if it causes error
		printf("could not initialised ttf support: %s\n", TTF_GetError());//print error message
		exit(EXIT_FAILURE);//exit
	}


	//filter events
	SDL_SetEventFilter(EventFilter, NULL);


	//quit SDL at exit
	atexit(Quit);



	Resize();//get display


	//Create window
	window = SDL_CreateWindow(
		WINDOW_NAME,                  // window title
		SDL_WINDOWPOS_CENTERED,           // initial x position
		SDL_WINDOWPOS_CENTERED,           // initial y position
		width,                               // width, in pixels
		height,                               // height, in pixels
		WINDOWFLAGS        //use these flags
		);


	// Check that the window was made
	if (window == NULL) {
		// In the event that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);//exit
	}


	//Create renderer
	renderer = SDL_CreateRenderer(window, -1, 0);


	// Check that the renderer was made
	if (renderer == NULL) {
		// In the event that the window could not be made...
		printf("Could not create renderer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);//exit
	}


	//initialise and load stuff
	delay = DELAY;//10 ms delay
	somethingwentwrong = GetTextTexture(font_64, "somethingwentwrong", 0, 0, 0);//image to display if something went wrong

	//loading message
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
	SDL_RenderClear(renderer);//clear screen
	SDL_Texture *loading = GetTextTexture(font_16, "Loading", 0, 0, 0);//loading message
	DrawText(loading, 0.5 * ws, 0.5 * hs, NULL, 1);//draw loading message
	SDL_RenderPresent(renderer);//present loading message
	SDL_DestroyTexture(loading);//don't need this texture

	//set initial values
	displaymode = DATA;//reset display
	Reset();//reset graph data
	xshift = yshift = XShiftAll = YShiftAll = 0;//set shifts to 0

	//load textures
	PointIMG = GetTexture(POINT);//get point image
	Resize();//reload display




	//main loop
	while (1) {
		while (SDL_PollEvent(&e)) {//for each e
			char str[1024];//string to fill for values
			switch (e.type) {//for each event type
			case SDL_QUIT://quit everything
				Quit();//quit everything
				break;//get out
			case SDL_MOUSEBUTTONDOWN://when clicking down
				if (e.button.which != SDL_TOUCH_MOUSEID){//if not touch event
					MouseX = (double)(e.button.x) / maxside - XShiftAll;//set x and y position of mouse from square normalised
					MouseY = (double)(e.button.y) / maxside - YShiftAll;
					Clicked();//run clicked function 
					Draged();//run draged function 
				}
				break;//get out
			case  SDL_FINGERDOWN://when finger touched
				MouseX = (double)(e.tfinger.x) * ws - YShiftAll;//set x and y position of mouse from square normalised
				MouseY = (double)(e.tfinger.y) * hs - XShiftAll;
				Clicked();//run clicked function 
				Draged();//run draged function 
				break;//get out
			case SDL_MOUSEMOTION://when mouse moved
				if (e.motion.which != SDL_TOUCH_MOUSEID){//if not touch event
					MouseX = (double)(e.motion.x) / maxside;//set x and y position of mouse from square normalised
					MouseY = (double)(e.motion.y) / maxside;
					if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){//if mouse is pressed down
						Draged();//run draged function 
					}
				}
				break;//get out
			case SDL_FINGERMOTION://when finger moved
					MouseX = (double)(e.tfinger.x) * ws - YShiftAll;//set x and y position of mouse from square normalised
					MouseY = (double)(e.tfinger.y) * hs - XShiftAll;
					Draged();//run draged function 
				break;//get out
			case SDL_WINDOWEVENT://when window was changed
				Resize();//resize stuff
				displayd = 0;//redraw
				break;//get out
			case SDL_TEXTINPUT://when text is inputed
				if (selected == 0) SDL_StopTextInput();//stop text input if nothing is selected
				else if (selected == 1) {//title
					if (strlen(title) > 200) break;//if selected value get's too big
					strcat(title, e.text.text);//get text
					displayd = 0;//redraw everything
				}
				else if (selected == 2) {//x name
					if (strlen(xname) > 200) break;//if selected value get's too big
					strcat(xname, e.text.text);//get text
					displayd = 0;//redraw everything
				}
				else if (selected == 3) {//y name
					if (strlen(yname) > 200) break;//if selected value get's too big
					strcat(yname, e.text.text);//get text
					displayd = 0;//redraw everything
				}
				else{//data
					if (strlen(data[selected - 4]) > 200) break;//if selected value get's too big
					strcat(data[selected - 4], e.text.text);//get text
					displayd = 0;//redraw everything
				}
				break;//get out
			case SDL_KEYDOWN://key is down
				if (e.key.keysym.sym == SDLK_BACKSPACE){//if backspace is pressed
					if (selected == 0) SDL_StopTextInput();//stop text input if nothing is selected
					else if (selected == 1) {//title
						if (strlen(title) <= 0) break;//deleted too much
						title[strlen(title) - 1] = '\0';//clear last caracter
						displayd = 0;//redraw everything
					}
					else if (selected == 2) {//x name
						if (strlen(xname) <= 0) break;//deleted too much
						xname[strlen(xname) - 1] = '\0';//clear last caracter
						displayd = 0;//redraw everything
					}
					else if (selected == 3) {//y name
						if (strlen(yname) <= 0) break;//deleted too much
						yname[strlen(yname) - 1] = '\0';//clear last caracter
						displayd = 0;//redraw everything
					}
					else{//data
						if (strlen(data[selected - 4]) <= 0) break;//deleted too much
						data[selected - 4][strlen(data[selected - 4]) - 1] = '\0';//clear last caracter
						displayd = 0;//redraw everything
					}
				}
				else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_RETURN2 || e.key.keysym.sym == SDLK_KP_ENTER){//if return is pressed
					SDL_StopTextInput();//stop text input events
					selected = 0;//unselect
					displayd = 0;//redraw everything
				}
				else if (e.key.keysym.sym == SDLK_TAB){//if tab is pressed
					SDL_StartTextInput();//start text input events
					selected = selected++%43;//select next value
					if (selected == 0) SDL_StopTextInput();//stop text input events
					displayd = 0;//redraw everything
				}
				break;//get out
			default://for everything else
				//ignore event
				break;//get out
			}
		}






		Draw();//draw screen
		if (difftime(time(NULL), lasttime) >= 1){//if 1 second passed
			fps = frame - lastframe;//set fps
			lasttime = time(NULL);//set new lasttime
			lastframe = frame;
#ifdef FPS
			printf("%d fps\n", fps);
#endif
		}
		frame++;//increment frame

		SDL_Delay(delay);//delay some time
	}

	exit(EXIT_SUCCESS);//if it somehow reaches here
}

















int EventFilter(void* userdata, SDL_Event* e){//event filter
	switch (e->type) {//for each event type
	case SDL_QUIT://quit everything
		exit(EXIT_SUCCESS);//exit
		return 0;//delete that event
		break;//get out

	case SDL_APP_TERMINATING://quit everything
		exit(EXIT_SUCCESS);//exit
		return 0;//delete that event
		break;//get out

	case SDL_APP_LOWMEMORY://if low in memory
		//can't do anything
		return 0;//delete that event
		break;//get out

	case SDL_APP_WILLENTERBACKGROUND://if entering background
		delay = DELAY_SLOW;//lower fps
		return 0;//delete that event
		break;//get out

	case SDL_APP_DIDENTERBACKGROUND://if in background
		//don't do anything
		return 0;//delete that event
		break;//get out

	case SDL_APP_WILLENTERFOREGROUND://if entering forgeound
		delay = DELAY;//higher fps
		return 0;//delete that event
		break;//get out

	case SDL_APP_DIDENTERFOREGROUND://if in forground
		//don't do anything
		return 0;//delete that event
		break;//get out

	default://for everything else
		return 1;//don't delete event
		break;//get out
	}
	return 1;//just in case something wiard happens
}




















void Quit(void){//quit everything
	//destroy textures
	SDL_DestroyTexture(PointIMG);



	free(Xdata);//free data
	free(Ydata);



	TTF_CloseFont(font_4);//close all fonts
	TTF_CloseFont(font_6);
	TTF_CloseFont(font_8);
	TTF_CloseFont(font_16);
	TTF_CloseFont(font_24);
	TTF_CloseFont(font_32);
	TTF_CloseFont(font_46);
	TTF_CloseFont(font_64);
	SDL_DestroyRenderer(renderer);//destroy renderer
	SDL_DestroyTexture(somethingwentwrong);//destroy something went wrong texture
	SDL_DestroyWindow(window);//destroy window
	IMG_Quit();//quit SDL_Image
	TTF_Quit();//quit SDL_TTF
	SDL_Quit();//quit SDL
	
	return;//exit function if it didn't exit for some reason
}

















void GetDisplay(void){//get display
	SDL_DisplayMode display;//display mode 
	if (window == NULL){
		//get desplay mode of the desktop if there is no window yet
		if (SDL_GetDesktopDisplayMode(0, &display) != 0) {
			printf("Could not get display mode: %s\n", SDL_GetError());//take care of errors
			exit(EXIT_FAILURE);//exit
		}
		width = WIDTH;//set display width and height
		height = HEIGHT;
	}
	else{
		//get desplay mode of the window if there is window
		SDL_GetWindowSize(window, &width, &height);//get width and height
	}

	if (width > height){//get smallest of 2 sides
		maxside = height;
	}
	else {
		maxside = width;
	}

	ws = (double)width / (double)maxside;//get width and height 
	hs = (double)height / (double)maxside;

	return;//exit function
}




















void Clicked(void){//x and y positions clicked
	switch (displaymode){//switch for each thing to display
	case DATA:
		if (MouseY > hs - 1.0 / 16 && MouseY < 1.0*hs){//if click is low enough
			if (MouseX < 0.12*ws){//if clicked clear
				Reset();//reset data
				displayd = 0;
				break;
			}
			if (MouseX > 0.88*ws){//if clicked graph
				displaymode = GRAPH;//go back to graph
				SDL_StopTextInput();//stop text input events
				selected = 0;//unselect

				int i;
				for (i = 0; i < 40; i++){//for each data
					if (data[i][0] == '0' ||
						data[i][0] == '1' ||
						data[i][0] == '2' ||
						data[i][0] == '3' ||
						data[i][0] == '4' ||
						data[i][0] == '5' ||
						data[i][0] == '6' ||
						data[i][0] == '7' ||
						data[i][0] == '8' ||
						data[i][0] == '9' || (
						data[i][0] == '.' && (
						data[i][1] == '0' ||
						data[i][1] == '1' ||
						data[i][1] == '2' ||
						data[i][1] == '3' ||
						data[i][1] == '4' ||
						data[i][1] == '5' ||
						data[i][1] == '6' ||
						data[i][1] == '7' ||
						data[i][1] == '8' ||
						data[i][1] == '9'
						)));//keep going if starting with a number
					else {//if not starting with a number
						break;
					}
				}
				length = (i+1)/2;//set length to i + 1 divided by 2

				Xdata = malloc(sizeof(double) * length);//allocate memory for length
				Ydata = malloc(sizeof(double) * length);

				for (i = 0; i < length; i++){//for each data
					Xdata[i] = atof(data[2 * i]);//set x data
					Ydata[i] = atof(data[2 * i + 1]);//set y data
				}

				displayd = 0;
				break;
			}
		}
		else if (MouseY < 1.0 / 24 && MouseY > 0.0){//if clicked on top
			if (MouseX > 0.35*ws && MouseX < 0.65*ws){//if in right range for title
				SDL_StartTextInput();//start text input events
				selected = 1;//select title
				displayd = 0;
				break;
			}
			else{
				SDL_StopTextInput();//stop text input events
				selected = 0;//unselect title
				YShiftAll = 0;//undo y shift
				displayd = 0;
				break;
			}
		}
		else if (MouseY < 1.0/12*hs){//if clicked on titles
			if (MouseX > 0.125*ws && MouseX < 0.375*ws){//if in right range for x name
				SDL_StartTextInput();//start text input events
				selected = 2;//select title
				displayd = 0;
				break;
			}
			else if (MouseX > 0.625*ws && MouseX < 0.875*ws){//if in right range for y name
				SDL_StartTextInput();//start text input events
				selected = 3;//select title
				displayd = 0;
				break;
			}
			else{
				SDL_StopTextInput();//stop text input events
				selected = 0;//unselect title
				YShiftAll = 0;//undo y shift
				displayd = 0;
				break;
			}
		}
		else if (MouseY < hs - 1.0 / 16){
			if (MouseX > 0.125*ws && MouseX < 0.375*ws){//if in right range for x data
				SDL_StartTextInput();//start text input events
				selected = (int)((MouseY/hs - 1.0/12) * 24)*2 + 4;//select title
				displayd = 0;
				break;
			}
			else if (MouseX > 0.625*ws && MouseX < 0.875*ws){//if in right range for y data
				SDL_StartTextInput();//start text input events
				selected = (int)((MouseY/hs - 1.0 / 12) * 24) * 2 + 5;//select title
				displayd = 0;
				break;
			}
			else{
				SDL_StopTextInput();//stop text input events
				selected = 0;//unselect title
				YShiftAll = 0;//undo y shift
				displayd = 0;
				break;
			}
		}
		break;
	case GRAPH:
		if (MouseY < 1.0 / 16 + 1.0 / 12 && MouseY > 0.0){//if click is high enough
			if (MouseX < 0.12*ws && MouseY < 1.0 / 16 && MouseY > 0.0){//if clicked back
				displaymode = DATA;//go back to data
				displayd = 0;
				break;
			}
			else if (MouseX < 0.55*ws){//if on left
				if (MouseY < 1.0 / 24 && MouseY > 0.0){//if on top
					xpow += 1;//increment x power by 1
					displayd = 0;
					break;
				}
				else if (MouseY < 1.0 / 24 + 1.0 / 16){//if on middle
					xinv = !xinv;//flip inverse
					displayd = 0;
					break;
				}
				else {//if on bottom
					if (xpow > 1) xpow -= 1;//decrement x power by 1
					displayd = 0;
					break;
				}
			}
			else {//if on right
				if (MouseY < 1.0 / 24 && MouseY > 0.0){//if on top
					ypow += 1;//increment y power by 1
					displayd = 0;
					break;
				}
				else if (MouseY < 1.0 / 24 + 1.0 / 16){//if on middle
					yinv = !yinv;//flip inverse
					displayd = 0;
					break;
				}
				else {//if on bottom
					if (ypow > 1) ypow -= 1;//decrement y power by 1
					displayd = 0;
					break;
				}
			}
		}
		displayd = 0;
		break;
	}
	return;//exit function
}



















void Draged(void){

	return;//exit function
}























SDL_Texture* GetTexture(const char *file){//make texture from this file
	char image[256] = RESOURCES;//folder path
	strcat(image, file);//append path
	SDL_Surface* surface = IMG_Load(image);//load surface
	if (surface == NULL){//if it could not be loaded
		printf("could not load image: %s\n", IMG_GetError());//error message
		return somethingwentwrong;//something went wrong
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);//get texture from loaded image
	if (texture == NULL){//if it could not be converted to texture
		printf("could not create texture: %s\n", SDL_GetError());//error message
		return somethingwentwrong;//something went wrong
	}
	SDL_FreeSurface(surface);//free surface
	return texture;//return texture
}

























TTF_Font* GetFont(const char *file, int size){//get font from file
	char fontfile[256] = RESOURCES;//folder path
	strcat(fontfile, FONTS);//append fonts path
	strcat(fontfile, file);//append path
	TTF_Font *font = TTF_OpenFont(fontfile, size);//get font
	if (font == NULL){//if it could not be loaded
		printf("could not load font: %s\n", TTF_GetError());//error message
		exit(EXIT_FAILURE);//exit
	}
	return font;//return font
}



























SDL_Texture* GetTextTexture(TTF_Font* font, const char* text, int r, int g, int b){//make texture from font text and rgb
	SDL_Color color;//set color
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = 255;
	char texturetext[256];//text to display for texture
	strcpy(texturetext, text);//copy text
	strtok(texturetext, "\n");//remove newline
	if (strcmp(texturetext, "") == 0){//if blank
		strcpy(texturetext, " ");//add space to avoid error
	}
	SDL_Surface* surface = TTF_RenderText_Blended(font, texturetext, color);//rendere text as surface
	if (surface == NULL){//if it could not be loaded
		printf("could not load text: %s\n", TTF_GetError());//error message
		exit(EXIT_FAILURE);//exit
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);//get texture from loaded image
	if (texture == NULL){//if it could not be converted to texture
		printf("could not create texture: %s\n", SDL_GetError());//error message
		exit(EXIT_FAILURE);//exit
	}
	SDL_FreeSurface(surface);//free surface
	return texture;//return created texture
}

























void Resize(void){//recalculate numbers related to size and load texts
	GetDisplay();//get display



	TTF_CloseFont(font_4);//close all fonts
	TTF_CloseFont(font_6);
	TTF_CloseFont(font_8);
	TTF_CloseFont(font_16);
	TTF_CloseFont(font_24);
	TTF_CloseFont(font_32);
	TTF_CloseFont(font_46);
	TTF_CloseFont(font_64);

	//load fonts on different sizes (1/n of maxside)
	font_4 = GetFont(FONT, maxside / 4);
	font_6 = GetFont(FONT, maxside / 6);
	font_8 = GetFont(FONT, maxside / 8);
	font_16 = GetFont(FONT, maxside / 16);
	font_24 = GetFont(FONT, maxside / 24);
	font_32 = GetFont(FONT, maxside / 32);
	font_46 = GetFont(FONT, maxside / 46);
	font_64 = GetFont(FONT, maxside / 64);


	if (renderer != NULL){//if there is a renderer
		
	}
}























void DrawBase(void){//draw basic stuff
}



























void DrawText(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center){//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size
	if (texture == NULL) {//if texture passed dosen't exist
		texture = somethingwentwrong;//set texture to something went wrong
	}
	SDL_Rect dest;
	int w, h, access;//value to fill up
	long format;
	SDL_QueryTexture(texture, &format, &access, &w, &h);//get text box size
	dest.w = (int) w;//set width and height
	dest.h = (int) h;
	dest.x = (int)(x * maxside);//set x and y
	dest.y = (int)(y * maxside);

	if (center){
		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
		dest.y = dest.y - dest.h / 2;
	}

	dest.x += XShiftAll * maxside;//shift x and y
	dest.y += YShiftAll * maxside;

	SDL_RenderCopy(renderer, texture, rect, &dest);//draw texture

}






void DrawTextAngled(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center, double deg){//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size at an angle
	if (texture == NULL) {//if texture passed dosen't exist
		texture = somethingwentwrong;//set texture to something went wrong
	}
	SDL_Rect dest;
	int w, h, access;//value to fill up
	long format;
	SDL_QueryTexture(texture, &format, &access, &w, &h);//get text box size
	dest.w = (int)w;//set width and height
	dest.h = (int)h;
	dest.x = (int)(x * maxside);//set x and y
	dest.y = (int)(y * maxside);

	if (center){
		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
		dest.y = dest.y - dest.h / 2;
	}

	dest.x += XShiftAll * maxside;//shift x and y
	dest.y += YShiftAll * maxside;

	SDL_RenderCopyEx(renderer, texture, rect, &dest, deg, NULL, SDL_FLIP_NONE);//draw texture
}




void DrawTextAngledCentered(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int centerx, int centery, double deg){//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size at an angle
	if (texture == NULL) {//if texture passed dosen't exist
		texture = somethingwentwrong;//set texture to something went wrong
	}
	SDL_Rect dest;
	int w, h, access;//value to fill up
	long format;
	SDL_QueryTexture(texture, &format, &access, &w, &h);//get text box size
	dest.w = (int)w;//set width and height
	dest.h = (int)h;
	dest.x = (int)(x * maxside);//set x and y
	dest.y = (int)(y * maxside);

	if (centerx){
		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
	}
	if (centery){
		dest.y = dest.y - dest.h / 2; // set x and y centered to x and y
	}

	if (deg == 90 || deg == 270){//if sideways
		dest.y = dest.y - dest.h / 2 + dest.w / 2;//adjust
	}

	dest.x += XShiftAll * maxside;//shift x and y
	dest.y += YShiftAll * maxside;

	SDL_RenderCopyEx(renderer, texture, rect, &dest, deg, NULL, SDL_FLIP_NONE);//draw texture
}



void DrawTextAngledCenteredEdge(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int centerx, int centery, double deg){//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size at an angle. Use x and y at other side
if (texture == NULL) {//if texture passed dosen't exist
	texture = somethingwentwrong;//set texture to something went wrong
}
SDL_Rect dest;
int w, h, access;//value to fill up
long format;
SDL_QueryTexture(texture, &format, &access, &w, &h);//get text box size
dest.w = (int)w;//set width and height
dest.h = (int)h;
dest.x = (int)(x * maxside) - w;//set x and y
dest.y = (int)(y * maxside) - h;

if (centerx){
	dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
}
if (centery){
	dest.y = dest.y - dest.h / 2; // set x and y centered to x and y
}

dest.x += XShiftAll * maxside;//shift x and y
dest.y += YShiftAll * maxside;

SDL_RenderCopyEx(renderer, texture, rect, &dest, deg, NULL, SDL_FLIP_NONE);//draw texture
}



void DrawTextSelected(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center, int sel){//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size. Add curser at end if selected value matches
	if (texture == NULL) {//if texture passed dosen't exist
		texture = somethingwentwrong;//set texture to something went wrong
	}
	SDL_Rect dest;
	int w, h, access;//value to fill up
	long format;
	SDL_QueryTexture(texture, &format, &access, &w, &h);//get text box size
	dest.w = (int)w;//set width and height
	dest.h = (int)h;
	dest.x = (int)(x * maxside);//set x and y
	dest.y = (int)(y * maxside);

	if (center){
		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
		dest.y = dest.y - dest.h / 2;
	}

	dest.x += XShiftAll * maxside;//shift x and y
	dest.y += YShiftAll * maxside;

	if (selected == sel){//if selected
		char r, g, b, a;//rgba to fill
		SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);//get color
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);//set color to blue
		SDL_RenderDrawLine(renderer, dest.x + dest.w, dest.y - YShiftAll * maxside, dest.x + dest.w, dest.y + dest.h - YShiftAll * maxside);//draw curser
		SDL_SetRenderDrawColor(renderer, r, g, b, a);//reset color
	}

	SDL_RenderCopy(renderer, texture, rect, &dest);//draw texture
}





void DrawIMG(SDL_Texture *texture, double x, double y, SDL_Rect *rect, double w, double h, int center){//draw rect of texture at x and y position at scale from maxside normalised. Null rect for whole texture. set center to 1 to center to x and y
	if (texture == NULL) {//if texture passed dosen't exist
		texture = somethingwentwrong;//set texture to something went wrong
	}	SDL_Rect dest;
	dest.w = (int) (maxside * w);//set width and height
	dest.h = (int) (maxside * h);
	dest.x = (int)(x * maxside);//set x and y
	dest.y = (int)(y * maxside);

	if (center){
		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
		dest.y = dest.y - dest.h / 2;
	}

	dest.x += XShiftAll * maxside;//shift x and y
	dest.y += YShiftAll * maxside;

	SDL_RenderCopy(renderer, texture, rect, &dest);//draw texture

}

void Draw(void){//draw/update screen
	SDL_Texture *Buttons;//button text
	char string[2048];//string to fill
	int i;
	switch (displaymode){//switch for each thing to display
	case DATA:
#ifdef MOBILE
		if (selected > 30){//if at bottom
			YShiftAll = -0.3;//shift up
		}
		else {
			YShiftAll = 0.0;// unshift
		}
#endif
		if (displayd) break;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image

		SDL_Texture *Texture = GetTextTexture(font_24, title, 0, 0, 0);//title
		DrawTextSelected(Texture, 0.5*ws, 1.0 / 48, NULL, 1, 1);//display title
		SDL_DestroyTexture(Texture);//destroy texture

		Texture = GetTextTexture(font_24, xname, 0, 0, 0);//xname
		DrawTextSelected(Texture, 0.25*ws, ((1.0 / 48) * 3)*hs, NULL, 1, 2);//display xname
		SDL_DestroyTexture(Texture);//destroy texture

		Texture = GetTextTexture(font_24, yname, 0, 0, 0);//yname
		DrawTextSelected(Texture, 0.75*ws, ((1.0 / 48) * 3)*hs, NULL, 1, 3);//display yname
		SDL_DestroyTexture(Texture);//destroy texture

		Texture = GetTextTexture(font_16, "Graph", 0, 0, 0);//graph button
		DrawText(Texture, 0.92*ws, hs - 1.0 / 32, NULL, 1);//display graph button
		SDL_DestroyTexture(Texture);//destroy texture

		Texture = GetTextTexture(font_16, "Clear", 0, 0, 0);//clear button
		DrawText(Texture, 0.08*ws, hs - 1.0 / 32, NULL, 1);//display clear button
		SDL_DestroyTexture(Texture);//destroy texture

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);//set color to black
		SDL_RenderDrawLine(renderer, 0.5*ws*maxside, 1.0 / 24 * hs*maxside, 0.5*ws*maxside, 1.0 / 12 * hs*maxside + (1.0 / 24) * 20 * hs*maxside);//draw line in middle
		int i;
		for (i = 0; i < 40; i++){
			if (!(i % 2)){
				SDL_RenderDrawLine(renderer, 0, 1.0 / 12 * hs*maxside + (1.0 / 24)*(i / 2)*hs*maxside, ws*maxside, 1.0 / 12 * hs*maxside + (1.0 / 24)*(i / 2)*hs*maxside);//draw line for each row
			}
			Texture = GetTextTexture(font_24, data[i], 0, 0, 0);// data
			DrawTextSelected(Texture, 0.25 * ws + (0.5 * ws) * (i%2), ((1.0/48) * 5 + (1.0/24)*(i/2))*hs, NULL, 1, i + 4);//display data
			SDL_DestroyTexture(Texture);//destroy texture
		}
		SDL_RenderDrawLine(renderer, 0, 1.0 / 12 * hs*maxside + (1.0 / 24)*20*hs*maxside, ws*maxside, 1.0 / 12 * hs*maxside + (1.0 / 24)*20*hs*maxside);//draw bottom line


		displayd = 1;//set displayd
		SDL_RenderPresent(renderer);//present rendered
		break;
	case GRAPH:
		if (displayd) break;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image
		
		sprintf(string, "Back");//back button
		SDL_Texture *Back = GetTextTexture(font_16, string, 0, 0, 0);//Back button
		DrawText(Back, 0.08*ws, 1.0 / 32, NULL, 1);//display Back button
		SDL_DestroyTexture(Back);//destroy texture


		sprintf(string, "^");//format axis manipulator
		SDL_Texture *XU = GetTextTexture(font_24, string, 0, 0, 0);//x axis manipulator
		DrawTextAngled(XU, 0.3*ws, 1.0 / 48, NULL, 1, 0);//display x axis manipulator
		SDL_DestroyTexture(XU);//destroy texture

		sprintf(string, "^");//format axis manipulator
		SDL_Texture *XD = GetTextTexture(font_24, string, 0, 0, 0);//x axis manipulator
		DrawTextAngled(XD, 0.3*ws, 1.0 / 24 + 1.0/16 + 1.0 / 48, NULL, 1, 180);//display x axis manipulator
		SDL_DestroyTexture(XD);//destroy texture

		sprintf(string, "^");//format axis manipulator
		SDL_Texture *YU = GetTextTexture(font_24, string, 0, 0, 0);//y axis manipulator
		DrawTextAngled(YU, 0.8*ws, 1.0 / 48, NULL, 1, 0);//display y axis manipulator
		SDL_DestroyTexture(YU);//destroy texture

		sprintf(string, "^");//format axis manipulator
		SDL_Texture *YD = GetTextTexture(font_24, string, 0, 0, 0);//y axis manipulator
		DrawTextAngled(YD, 0.8*ws, 1.0 / 24 + 1.0 / 16 + 1.0 / 48, NULL, 1, 180);//display y axis manipulator
		SDL_DestroyTexture(YD);//destroy texture

		sprintf(string, "%s(%s)^%.0f", xinv ? "1/" : "", xname, xpow);//format axis manipulator
		SDL_Texture *Xdisplay = GetTextTexture(font_16, string, 0, 0, 0);//x axis manipulator
		DrawText(Xdisplay, 0.3*ws, 1.0 / 36 + 1.0 / 24, NULL, 1);//display x axis manipulator
		SDL_DestroyTexture(Xdisplay);//destroy texture

		sprintf(string, "%s(%s)^%.0f", yinv ? "1/" : "", yname, ypow);//format axis manipulator
		SDL_Texture *Ydisplay = GetTextTexture(font_16, string, 0, 0, 0);//y axis manipulator
		DrawText(Ydisplay, 0.8*ws, 1.0 / 36 + 1.0 / 24, NULL, 1);//display y axis manipulator
		SDL_DestroyTexture(Ydisplay);//destroy texture

		Graph(title, 0, 1.0/16 + 1.0/12, ws - (1.0/24), hs - (1.0/16 + 1.0/12), xname, yname, Xdata, Ydata, length, xpow, ypow, xinv, yinv);//test graph

		displayd = 1;//set displayd
		SDL_RenderPresent(renderer);//present rendered
		break;
	}
}

void Graph(const char* title, double x, double y, double w, double h, const char* xn, const char* yn, const double* xd, const double* yd, int dl, double xp, double yp, int xin, int yin){//draw graph with title starting from x and y position normalised at scale from maxside, and with w and h width and height with data name of xn and yn, and data of xd, and yd with length of dl. data will be modified by raising it to the power of xp and yp and making it inverse if xp or yp is 1.
	char name[2048];//name of variable
	
	SDL_Texture *Title = GetTextTexture(font_24, title, 0, 0, 0);//title
	DrawText(Title, x + 0.5*w, y + 1.0 / 24 / 2, NULL, 1);//display title
	SDL_DestroyTexture(Title);//destroy texture

	sprintf(name, "%s(%s)^%.0f", xin ? "1/" : "", xn, xp);//format axis name
	SDL_Texture *Xname = GetTextTexture(font_24, name, 0, 0, 0);//name of x axis
	DrawText(Xname, x + 0.5*w, y + h - (1.0/24/2)*3, NULL, 1);//display name of x axis
	SDL_DestroyTexture(Xname);//destroy texture

	sprintf(name, "%s(%s)^%.0f", yin ? "1/" : "", yn, yp);//format axis name
	SDL_Texture *Yname = GetTextTexture(font_24, name, 0, 0, 0);//name of y axis
	DrawTextAngled(Yname, x + 1.0 / 24 / 2, y + 0.5*h, NULL, 1, 270);//display name of y axis sideways
	SDL_DestroyTexture(Yname);//destroy texture

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);//set color to black
	SDL_RenderDrawLine(renderer, (x + 1.0 / 24)*maxside, (y + h - (1.0 / 24) * 2)*maxside, (x + 1.0 / 24)*maxside, (y + 1.0 / 24)*maxside);//draw y axis
	SDL_RenderDrawLine(renderer, (x + 1.0 / 24)*maxside, (y + h - (1.0 / 24) * 2)*maxside, (x + w)*maxside, (y + h - (1.0 / 24) * 2)*maxside);//draw x axis
	double *xdata = malloc(sizeof(double)*dl);//allocate memory for xdata
	xdata = memcpy(xdata, xd, sizeof(double)*dl);//get copy of x data
	double *ydata = malloc(sizeof(double)*dl);//allocate memory for ydata
	ydata = memcpy(ydata, yd, sizeof(double)*dl);//get copy of x data
	int i;
	for (i = 0; i < dl; i++){//for each x and y value
		xdata[i] = pow(xdata[i], xp);//raise to power
		ydata[i] = pow(ydata[i], yp);//raise to power
		if (xin){//if it is to be inversed
			xdata[i] = 1.0 / xdata[i];//inverse data
		}
		if (yin){//if it is to be inversed
			ydata[i] = 1.0 / ydata[i];//inverse data
		}
		

	}
	double resulta = 0, resultb = 0, acc = 0;//a and b of y = ax + b and accurecy in r^2
	double maxX = 0, maxY = 0;//maximum x and y
	if (dl > 1){//if there is more then 1 data
		double Xt = 0, Yt = 0, XYt = 0, X2t = 0;//x total, y total, x*y total, x^2 total
		if (dl) {
			maxX = xdata[0];//initialise maximum x and y
			maxY = ydata[0];
		}
		for (i = 0; i < dl; i++){//for each x and y value
			if (xdata[i] > maxX) maxX = xdata[i];//get x and y max value
			if (ydata[i] > maxY) maxY = ydata[i];
			Xt += xdata[i];//add up data
			Yt += ydata[i];
			XYt += xdata[i] * ydata[i];
			X2t += xdata[i] * xdata[i];
		}
		resulta = (XYt / dl - (Xt / dl)*(Yt / dl)) / (X2t / dl - (Xt / dl)*(Xt / dl));//get a
		resultb = Yt / dl - resulta*(Xt / dl);//get b 
		double tot = 0, res = 0;//values needed for r^2
		for (i = 0; i < dl; i++){//for each x and y value
			tot += (ydata[i] - Yt / dl)*(ydata[i] - Yt / dl);//total sum of squares
			res += (ydata[i] - (xdata[i] * resulta + resultb))*(ydata[i] - (xdata[i] * resulta + resultb));//residual sum of squares
		}
		acc = 1 - res / tot;
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);//set color to green
		SDL_RenderDrawLine(renderer, ((x + 1.0 / 24) + (0 / maxX) * (w - (1.0 / 24)))*maxside, ((y + h - (1.0 / 24) * 2) - (resultb / maxY)  * (h - (1.0 / 24) * 3))*maxside, ((x + 1.0 / 24) + (maxX / maxX) * (w - (1.0 / 24)))*maxside, ((y + h - (1.0 / 24) * 2) - ((maxX*resulta + resultb) / maxY)  * (h - (1.0 / 24) * 3))*maxside);//draw line of best fit
	}
	for (i = 0; i < dl; i++){//for each x and y value
		//plot points
		DrawIMG(PointIMG, (x + 1.0 / 24) + (xdata[i] / maxX) * (w - (1.0 / 24)) , (y + h - (1.0 / 24) * 2) - (ydata[i] / maxY)  * (h - (1.0 / 24) * 3) , NULL, POINT_SIZE, POINT_SIZE, 1);//draw point
	}
	sprintf(name, "R^2 = %.5f           %s(%s)^%.0f = %.3e(%s(%s)^%.0f)%+.3e", acc, yin ? "1/" : "", yn, yp, resulta, xin ? "1/" : "", xn, xp, resultb);//format equations
	SDL_Texture *Result = GetTextTexture(font_24, name, 0, 0, 0);//equations
	DrawText(Result, x + 0.5*w, y + h - 1.0 / 24 / 2, NULL, 1);//display equations
	SDL_DestroyTexture(Result);//destroy texture

	SDL_Texture *Zero = GetTextTexture(font_24, "0", 0, 0, 0);//zero
	DrawText(Zero, x + 1.0 / 48, y + h - ((1.0 / 48) * 3), NULL, 1);//display zero
	SDL_DestroyTexture(Zero);//destroy texture

	sprintf(name, "%.3e", maxX);//format axis value
	SDL_Texture *XM = GetTextTexture(font_24, name, 0, 0, 0);//x axis value
	DrawTextAngledCenteredEdge(XM, x + w, y + h - (1.0 / 72) , NULL, 0, 1, 0);//display x axis value
	SDL_DestroyTexture(XM);//destroy texture

	sprintf(name, "%.3e", maxY);//format axis value
	SDL_Texture *YM = GetTextTexture(font_24, name, 0, 0, 0);//y axis value
	DrawTextAngledCentered(YM, x + (1.0 / 24 / 2), (y + 1.0 / 24), NULL, 1, 0, 270);//display y axis value sideways
	SDL_DestroyTexture(YM);//destroy texture
}

void Reset(void){//reset data and data manipulator
	xpow = 1;//reset data manipulator
	ypow = 1;
	xinv = 0;
	yinv = 0;
	free(Xdata);//free data
	Xdata = NULL;
	free(Ydata);
	Ydata = NULL;
	strcpy(xname, "X");//reset name to default
	strcpy(yname, "Y");
	strcpy(title, "Title");
	length = 0;//reset length
	selected = 0;//unselect
	int i;
	for (i = 0; i < 40; i++){//for each data
		strcpy(data[i], "");//clear data
	}
}
























































































