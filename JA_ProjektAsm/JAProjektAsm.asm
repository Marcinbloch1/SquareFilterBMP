.data

;deklaracja zmiennych
pixelNumber QWORD ?
rowNumber QWORD ?
pixelNumberMax QWORD ?
rowNumberMax QWORD ?
inputDataWidth QWORD ?
inputDataHeight QWORD ?
inputDataFin QWORD ?
inputData QWORD ?
redPixelVALue SDWORD ?
greenPixelVALue SDWORD ?
BLuePixelVALue SDWORD ?

.code
filterOn proc EXPORT

;zerowanie zmiennych inicjALizacja
xor RAX, RAX

mov redPixelVALue, EAX
mov greenPixelVALue, EAX
mov BLuePixelVALue, EAX
mov pixelNumber, RAX
mov rowNumber, RAX

mov RAX, RCX
mov inputData, RAX

mov RAX, RDX
mov RBX, 3
mul RBX
mov inputDataWidth, RAX

mov RAX, R8
mov inputDataHeight, RAX
mov inputDataFin, R9

mov RAX, inputDataWidth 

sub RAX, 6 
mov pixelNumberMax, RAX

mov RAX, inputDataHeight
sub RAX, 2
mov rowNumberMax, RAX

;iteracja po taBLicy (wiersze obrazu)
rowNumberLoop: 
	mov ECX, 0 
	mov EDI, 0 
	add rowNumber, 1 
	mov pixelNumber, 0

	;iteracja po taBLicy (kolumny obrazu)
	pixelNumberLoop: 
		add pixelNumber, 3 
		xor RAX, RAX 
		mov redPixelVALue, EAX 
		mov greenPixelVALue, EAX 
		mov BLuePixelVALue, EAX 

		mov R10, -1
		xLoop:

			mov R11, -3
			yLoop:

				;suma wartoœci pikseli red
				mov RAX, rowNumber
				add RAX, R10
				mul inputDataWidth
				add RAX, pixelNumber
				add RAX, R11
				add RAX, 0
				add RAX, inputData
				xor RBX, RBX
				mov BL, [RAX]
				xor RAX, RAX
				mov AL, BL
				add redPixelVALue, EAX

				;suma wartoœci pikseli green
				mov RAX, rowNumber
				add RAX, R10
				mul inputDataWidth
				add RAX, pixelNumber
				add RAX, R11
				add RAX, 1
				add RAX, inputData
				xor RBX, RBX
				mov RBX, [RAX]
				xor RAX, RAX
				mov AL, BL
				add greenPixelVALue, EAX

				;suma wartoœci pikseli blue
				mov RAX, rowNumber
				add RAX, R10
				mul inputDataWidth
				add RAX, pixelNumber
				add RAX, R11
				add RAX, 2
				add RAX, inputData
				xor RBX, RBX
				mov BL, [RAX]
				xor RAX, RAX
				mov AL, BL
				add BLuePixelVALue, EAX

			add R11, 3
			cmp R11D, 3
			jle yLoop

		inc R10
		cmp R10D, 1
		jle xLoop

		
		;po zsumowaniu wartosci trzeba je podzielic
		;DZIELENIE RED
		xor RAX, RAX 
		xor RBX, RBX 
		xor EAX, EAX 
		xor RCX, RCX 
		mov EAX, redPixelVALue ;wczytanie redPixelVALue do EAX
		xorps XMM0, XMM0 
		cvtsi2ss XMM0, EAX 
		mov RDX, 9 
		cvtsi2ss XMM1, RDX 
		divss XMM0, XMM1 
		cvtss2si EAX, XMM0 
		mov redPixelVALue, EAX ;wczytanie wyniku dzielenia do redPixelVALue

		;DZIELENIE GREEN
		xor RAX, RAX 
		xor RBX, RBX 
		xor EAX, EAX 
		xor RCX, RCX 
		mov EAX,greenPixelVALue ;wczytanie greenPixelVALue do EAX
		xorps XMM0, XMM0 
		cvtsi2ss XMM0, EAX 
		mov RDX, 9 
		cvtsi2ss XMM1, RDX 
		divss XMM0, XMM1 
		cvtss2si EAX, XMM0 
		mov greenPixelVALue,EAX ;wczytanie wyniku dzielenia do greenPixelVALue

		;DZIELENIE BLUE
		xor RAX, RAX 
		xor RBX, RBX 
		xor EAX, EAX 
		xor RCX, RCX 
		mov EAX,BLuePixelVALue ;wczytanie BLuePixelVALue do EAX
		xorps XMM0, XMM0 
		cvtsi2ss XMM0, EAX 
		mov RDX, 9 
		cvtsi2ss XMM1, RDX 
		divss XMM0, XMM1 
		cvtss2si EAX, XMM0 
		mov BLuePixelVALue,EAX ;wczytanie wyniku dzielenia do BLuePixelVALue

		;ZAPIS DO TABLICY
		mov RCX, 3 ;wczytanie 3 do RCX

		;ZAPIS WARTOSCI RED
		xor RAX, RAX 
		mov RAX, rowNumber ;wczytanie rowNumber do RAX
		mul inputDataWidth 
		add RAX, pixelNumber 
		add RAX, 0 
		add RAX, inputDataFin 
		xor RBX, RBX 
		mov EBX, redPixelVALue 
		mov [RAX], BL ;zapis do taBLicy

		;ZAPIS WARTOSCI GREEN
		xor RAX, RAX 
		mov RAX, rowNumber ;wczytanie rowNumber do RAX
		mul inputDataWidth 
		add RAX, pixelNumber 
		add RAX, 1 
		add RAX, inputDataFin 
		xor RBX, RBX 
		mov EBX, greenPixelVALue 
		mov [RAX], BL ;zapis do taBLicy

		;ZAPIS WARTOSCI BLUE
		xor RAX, RAX 
		mov RAX, rowNumber ;wczytanie rowNumber do RAX
		mul inputDataWidth 
		add RAX, pixelNumber 
		add RAX, 2 
		add RAX, inputDataFin 
		xor RBX, RBX 
		mov EBX, BLuePixelVALue 
		mov [RAX], BL ;zapis do taBLicy

		;przejscie do nastepnego piksela
		mov RAX, pixelNumber ;wczytanie pixelNumber do RAX
		cmp RAX, pixelNumberMax ;porownanie czy zostAL osiagniety max
		JB pixelNumberLoop ;jesli nie to skok do pixelNumberLoop

	mov RAX, rowNumber ;wczytanie rowNumber do RAX
	cmp RAX, rowNumberMax ;porownanie czy zostAL osiagniety max
	JB rowNumberLoop ;jesli nie to skok do rowNumberLoop

ret ;koniec procedury
filterOn endp
end