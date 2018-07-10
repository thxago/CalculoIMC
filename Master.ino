/*
Universidade Estadual de Maringá (UEM)
Departamento de Informática (DIN)
Disciplina: Sistemas Digitais
Aluno: Thiago Alberto RA: 82703
Professor Doutor Elvio João Leonardo
*/

//Biblioteca do teclado
#include <Keypad.h>
//Biblioteca de comunicação I2C/TWI
#include "Wire.h"

//Endereço do Escravo
#define ESCRAVO_ENDERECO 0x08

//número de linhas do teclado
const byte LINHAS = 4; 
//número de colunas do teclado
const byte COLUNAS = 4;

//conexão do teclado com o arduíno
byte linhaPinos[LINHAS]   = {9, 8, 7, 6}; 
byte colunaPinos[COLUNAS] = {5, 4, 3, 2}; 

//mapeia teclado conforme linha e coluna
char digitos[LINHAS][COLUNAS] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'.', '0', '.', 'D'}
};

//inicializa a instância teclado da classe Keypad
Keypad teclado = Keypad(makeKeymap(digitos), linhaPinos, colunaPinos, LINHAS, COLUNAS); 

void setup(){
	//barramento I2C
	Wire.begin();
}
  
void loop(){
  	//leitura do digito pressionado no teclado
	char digito = teclado.getKey();
  
	if (digito){
		//transmissao do digito para o endereco do escravo
		Wire.beginTransmission(ESCRAVO_ENDERECO);
		Wire.write(digito);
		Wire.endTransmission();
	}
}
