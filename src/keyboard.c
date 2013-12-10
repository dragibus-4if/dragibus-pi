#include "keyboard.h"


 u16 keyPressed [6] = { 0 , 0 , 0 , 0 , 0, 0 };

//char keysNormal [104];
//char keysShift [104];

//http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/input01.html



char keysNormal[104] = {
	0x00, 0x00, 0x00, 0x00, 'a', 'b', 'c', 'd',
	 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
	 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',
	'3', '4', '5', '6', '7', '8', '9', '0',
	'\n', 0x00, '\b', '\t', ' ', '-', '=', '[',
	 ']', '\\', '#', ';', '\'', '`', ',', '.',
	 '/', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, '/', '*', '-', '+',
	 '\n', '1', '2', '3', '4', '5', '6', '7',
	 '8', '9', '0', '.', '\\', 0x00, 0x00, '='
};

char keysShift [104] = {
	 0x00, 0x00, 0x00, 0x00, 'A', 'B', 'C', 'D',
	 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	 'U', 'V', 'W', 'X', 'Y', 'Z', '!', '"',
	 '£', '$', '%', '^', '&', '*', '(', ')',
	 '\n', 0x00, '\b', '\t', ' ', '_', '+', '{',
	 '}', '|', '~', ':', '@', '¬', '<', '>',
	 '?', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, '/', '*', '-', '+',
	 '\n', '1', '2', '3', '4', '5', '6', '7',
	 '8', '9', '0', '.', '|', 0x00, 0x00, '='
};

u32 kAddress = 0;

void start_driver(){
	led_off();
	UsbInitialise();
	while ( true ){
		keyBoardUpdate();
		keyBoardGetChar();
	}
	

}

void writeCharToPipe ( char c ){
	//printf("%c", c);
	if ( c == 0x00 ){
		//led_off();
	}
	else{
		//led_off();
	}
}

 int keyWasDown ( u16 key ){
	for ( int i = 0 ; i < 6 ; i ++ ){
		if ( keyPressed[i] == key ){
			return 0; // la touche était bien déja pressée
		}
	}
	return 1; // la touche est pressée pour la premiere fois
}

/*
* Cette méthode checke si de nouvelles touches ont été entrées et appelle 
* directement writeCharToPipe pour chacune d'entre elle
*/
void keyBoardGetChar(){
	if ( kAddress == 0 ){
		//led_on();
		return;
	}
	struct KeyboardLeds leds = {.NumberLock=true, .CapsLock=true, .ScrollLock=true, .Compose=true, .Kana=true, .Power=true, .Mute=true, .Shift=true};
	
	Result status = KeyboardSetLeds( kAddress, leds);
	if ( status != OK ){
		//led_on();	
	}
	for ( u32 i = 0 ; i < 6 ; i ++ ){
		u16 key = KeyboardGetKeyDown( kAddress, i );
		if ( key == 0 ){ // appels suivant index croissant ou decroissant ?
			//led_on();
			return ;
		}
		//led_off();
                if ( keyWasDown(key) !=0 ){ // lm'on ne traite la touche que si elle vient d'etre tapée
			if ( key <= 103 ) {
				struct KeyboardModifiers kMods = KeyboardGetModifiers ( kAddress );
			
				if ( kMods.LeftShift || kMods.RightShift ){ //on matche la touche trouvée avec la table keysShift
					char c = keysShift[ key ];
					writeCharToPipe( c );	
				}
				else{
					char c = keysNormal[ key ];
					writeCharToPipe( c );			
				}
			
				
			}
		}
	}
}

 void keyBoardUpdate(){
	if ( kAddress == 0 ){
		
		UsbCheckForChange();
		u32 kBoardCount = KeyboardCount();
		if ( kBoardCount == 0 ) {
			//led_on();
			return;
		}
		kAddress = KeyboardGetAddress( 0 ) ;
		if ( kAddress == 0 ){
			//led_on();
			return; // il y a un soucis ...
		}
		
	}
	//led_off();
	
	for ( u32 i = 0  ; i < 6 ; i ++ ){
		// pas sur de l'ordre dans lequel appeler 
                //cette méthode pour restituer l'ordre
	        // chronologique d'appui des touches ( index croissant ou décroissant)

		keyPressed[i] = KeyboardGetKeyDown( kAddress, i );
	}
	Result status = KeyboardPoll ( kAddress );
	if ( status != OK){
		//led_on();
	    	kAddress = 0;
	}
}


