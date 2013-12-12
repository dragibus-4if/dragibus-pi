#include "keyboard.h"


u16 keyPressed [6] = { 0 };

//char keysNormal [104];
//char keysShift [104];

//http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/input01.html

int cpt =0;

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



void writeCharToPipe ( char c ){
	//printf("%c", c);
	if ( c == 0x00 ){
	
		return;
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
		return;
	}
	//u32 nbKeysDown = KeyboardGetKeyDownCount( kAddress );
	//for ( u32 i = (u32)(0) ; i<= (u32)(3) ; i ++ ){
		u16 key = KeyboardGetKeyDown( kAddress, 0 );
		/*if ( key == (u16)(0) ){ // appels suivant index croissant ou decroissant ?
			led_on();
			cpt++;
			if ( cpt == 6) { led_off(); cpt=0}
			return ;
		} */
		
                //if ( keyWasDown(key) !=0 ){ // l'on ne traite la touche que si elle vient d'etre tapée
			
			if ( key <= 103 ) {
				//struct KeyboardModifiers kMods = KeyboardGetModifiers ( kAddress );
			
				//if ( kMods.LeftShift || kMods.RightShift ){ //on matche la touche trouvée avec la table keysShift
				//	char c = keysShift[ key ];
				//	writeCharToPipe( c );	
				//}
				//else{
					
					char c = keysNormal[ (int)(key) ];
					writeCharToPipe( c );			
				//}
			
				
			}
		//}
	//}
}

 void keyBoardUpdate(){

	if ( kAddress == 0 ){
		
		UsbCheckForChange(); //renvoie void -> pas de status à checker
		u32 kBoardCount = KeyboardCount();

		/*if ( kBoardCount == (u32)(0) ) {
			led_on(); while(true);
			return;
		}*/

		kAddress = KeyboardGetAddress( 0 ) ;
		if ( kAddress == 0 ){
			//led_on();
			return; // il y a un soucis ...
		}
		
	}
	
	struct KeyboardLeds leds = {.NumberLock=1, .CapsLock=0, .ScrollLock=0, .Compose=1, .Kana=1, .Power=0, .Mute=0, .Shift=0};
	
	/*Result status =*/ KeyboardSetLeds( kAddress, leds);
	/*if ( status != OK ){
		
	}*/
	
	
	 //led_on(); while(true);
	
	//u32 nbKeysDown = KeyboardGetKeyDownCount( kAddress );
	
	/*if ( /*nbKeysDown == (u32)(0) || nbKeysDown > (u32) (1000
) ){ led_on(); while(true); }*/
	
	u32 l = 2;
	for ( u32 i = 0  ; i < l ; i ++ ){
		// pas sur de l'ordre dans lequel appeler 
                //cette méthode pour restituer l'ordre
	        // chronologique d'appui des touches ( index croissant ou décroissant)
		keyPressed[(int)(i)] = KeyboardGetKeyDown( kAddress, i );	
	}
	
	Result status = KeyboardPoll ( kAddress );
	
	if ( status != OK){
	    	kAddress = 0;
		led_off();
	}
	
}




void start_driver(){
	
	/*Result res = */UsbInitialise();

	while ( true ){
		keyBoardUpdate();
		keyBoardGetChar();
	}

}

