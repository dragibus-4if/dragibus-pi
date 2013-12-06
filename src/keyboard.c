#include keyboard.h


private u16 keyPressed [6] = { 0 , 0 , 0 , 0 , 0, 0 };

private char keysNormal [104];
private char keysShift [104];

//http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/input01.html

keysNormal = {
	0x0, 0x0, 0x0, 0x0, 'a', 'b', 'c', 'd',
	 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
	 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',
	'3', '4', '5', '6', '7', '8', '9', '0',
	'\n', 0x0, '\b', '\t', ' ', '-', '=', '[',
	 ']', '\\', '#', ';', '\'', '`', ',', '.',
	 '/', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, '/', '*', '-', '+',
	 '\n', '1', '2', '3', '4', '5', '6', '7',
	 '8', '9', '0', '.', '\\', 0x0, 0x0, '='
};

keysShift = {
	 0x0, 0x0, 0x0, 0x0, 'A', 'B', 'C', 'D',
	 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
	 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	 'U', 'V', 'W', 'X', 'Y', 'Z', '!', '"',
	 '£', '$', '%', '^', '&', '*', '(', ')',
	 '\n', 0x0, '\b', '\t', ' ', '_', '+', '{',
	 '}', '|', '~', ':', '@', '¬', '<', '>',
	 '?', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, '/', '*', '-', '+',
	 '\n', '1', '2', '3', '4', '5', '6', '7',
	 '8', '9', '0', '.', '|', 0x0, 0x0, '='
};

private u32 kAddress = 0;

void start_driver(){
	UsbInitialise();
	while ( true ){
		keyBoardUpdate();
		keyBoardGetChar();
	}
	

}

private void writeCharToPipe ( char c ){
	printf("%c", c);
}

/*
* Cette méthode checke si de nouvelles touches ont été entrées et appelle 
* directement writeCharToPipe pour chacune d'entre elle
*/
pivate void keyBoardGetChar(){
	if ( kAddress == 0 ){
		return;
	}
	for ( u32 i = 0 ; i < 6 ; i ++ ){
		u16 key = KeyboardGetKeyDown( kAddress, i );
		if ( key == 0 ){ // appels suivant index croissant ou decroissant ?
			return ;		
		}
		if ( key <= 103 ) {
			KeyboardModifiers kMods = KeyboardGetModifiers ( kAddress );
			if ( kMods.LeftShift || kMods.RightShift ){ //on matche la touche trouvée avec la table keysShift
				char c = keysShift[ key ];	
			}
			else{
				char c = keysNormal[ key ];			
			}
			writeCharToPipe( c );
				
		}
	}
}

private void keyBoardUpdate(){
	if ( kAddress == 0 ){
		
		UsbCheckForChange();
		u32 kBoardCount = KeyboardCount();
		if ( kBoardCount == 0 ) {
			return;
		}
		kAddress = KeyboardGetAddress( 0 ) ;
		if ( kAddress == 0 ){
			return; // il y a un soucis ...
		}
		
	}
	for ( u32 i = 0  ; i < 6 ; i ++ ){
		// pas sur de l'ordre dans lequel appeler 
                //cette méthode pour restituer l'ordre
	        // chronologique d'appui des touches ( index croissant ou décroissant)

		keyPressed[i] = KeyboardGetKeyDown( kAddress, i );
	}
		Result status = KeyboardPoll ( kAddress );
		if ( status != 0){
		    	kAddress = 0;
		}
}

private int keyWasDown ( u16 key ){
	for ( int i = 0 ; i < 6 ; i ++ ){
		if ( keyPressed[i] == key ){
			return 1; // la touche était bien déja pressée
		}
	}
	return 0; // la touche est pressée pour la premiere fois
}
