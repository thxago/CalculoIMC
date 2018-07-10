/*
Universidade Estadual de Maringá (UEM)
Departamento de Informática (DIN)
Disciplina: Sistemas Digitais
Aluno: Thiago Alberto RA: 82703
Professor Doutor Elvio João Leonardo
*/

//Biblioteca do LCD
#include <LiquidCrystal.h>
//Biblioteca de comunicação I2C/TWI
#include "Wire.h"

//Endereço do Escravo
#define ESCRAVO_ENDERECO 0x08

//Pinos do LCD
#define LCD_READ_SELECT 2
#define LCD_ENABLE 3
#define LCD_DB4 4
#define LCD_DB5 5
#define LCD_DB6 6
#define LCD_DB7 7

//Pinos do LED
#define LED_VERDE 8
#define LED_AMARELO 9
#define LED_VERMELHO 10

//Estados do LED
#define LIGADO HIGH
#define DESLIGADO LOW

//VARIÁVEIS GLOBAIS
char pesoString[7];
char alturaString[7];
float altura = 0;
float peso = 0;
float imc = 0;
int iniciaPrograma = 0;
int posicaoPeso = 0;
int posicaoAltura = 0;
//variavel para verificar se o peso ja foi preenchido
bool pesoVerifica = false;

//configura o LCD
LiquidCrystal lcd(LCD_READ_SELECT, LCD_ENABLE, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);

void setup(){  
  //inicia barramento I2C
  Wire.begin(ESCRAVO_ENDERECO);

  //quando chega algum dado via I2C
  Wire.onReceive(registraEvento);
  
  //inicializa o LCD
  lcd.begin(16,2);
  telaPrincipal();
}

void loop(){}

void registraEvento(int valor) {
  // verifica se existe dado para ser lido no barramento I2C
  if (Wire.available()) {
    // le o byte recebido
	char byteRecebido = Wire.read();
    
    //Após ler as instruções, inicia-se o programa
    if(iniciaPrograma == 0){
      iniciaPrograma = 1;
      telaPeso();
    }else{
      //Preenche peso
      if(pesoVerifica == false){
        pesoInsere(byteRecebido);
      }else{//Preenche altura
        alturaInsere(byteRecebido);
      }
    }
  }
}

//Desliga possíveis LEDs ligados
void ledsDesligados(){
  digitalWrite(LED_AMARELO, DESLIGADO);
  digitalWrite(LED_VERMELHO, DESLIGADO);
  digitalWrite(LED_VERDE, DESLIGADO);
}

void telaPrincipal(){
  iniciaPrograma = 0;
  pesoVerifica = false;
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 0, linha 0;
  lcd.setCursor(0, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("A = Enter");
  //Posiciona o cursor na coluna 0, linha 1;
  lcd.setCursor(0, 1);
  //Envia o texto entre aspas para o LCD
  lcd.print("B = Apagar");
}

void telaPeso(){
  //Desliga Leds
  ledsDesligados();
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 0, linha 0;
  lcd.setCursor(0, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("Peso(kg): ");
  //Posiciona o cursor na coluna 0, linha 1;
  lcd.setCursor(0, 1);
}

void telaAltura(){
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 0, linha 0;
  lcd.setCursor(0, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("Altura(cm): ");
  //Posiciona o cursor na coluna 0, linha 1;
  lcd.setCursor(0, 1);
}

void telaIMC(){
  calculaIMC();
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 0, linha 0;
  lcd.setCursor(0, 0);
  //Envia as informações para o LCD
  lcd.print("IMC: ");
  lcd.print(imc);
  lcd.print("%");
  //Posiciona o cursor na coluna 0, linha 1;
  lcd.setCursor(0, 1);
  //Imprime a situação do IMC no LCD
  if(imc < 18.5){
    analogWrite(LED_VERMELHO, LIGADO);
    lcd.print("Abaixo do peso");
  }else if(imc < 25){
    analogWrite(LED_VERDE, LIGADO);
    lcd.print("Normal");
  }else if(imc < 30){
    analogWrite(LED_AMARELO, LIGADO);
    lcd.print("Sobrepeso");
  }else{
    analogWrite(LED_VERMELHO, LIGADO);
    lcd.print("Obesidade");
  }
  pesoVerifica = false;
  iniciaPrograma = 0;
}

//Realiza o cálculo do IMC
void calculaIMC(){
  //Converte char em flot
  peso = atof(pesoString);
  altura = atof(alturaString);
  limpaString();
  //Calcula % de IMC
  imc = (peso/(altura*altura))*10000;
}

//Esvazia Strings
void limpaString(){
  for(int i=7; i>=0; i--){
    pesoString[i] = '\0';
    alturaString[i] = '\0';
  }
}

//Recebe o peso
void pesoInsere(char valor){
  switch(valor){
    
    //ENTER
    case 'A':
    if(posicaoPeso > 0){
      posicaoPeso = 0;
      pesoVerifica = true;
      telaAltura();
    }
    break;
    
    //APAGA
    case 'B':
    if(posicaoPeso > 0){
      posicaoPeso--;
      pesoString[posicaoPeso] = '\0';
      //Limpa a tela
      lcd.clear();
      //Posiciona o cursor na coluna 0, linha 0;
      lcd.setCursor(0, 0);
      //Envia o texto entre aspas para o LCD
      lcd.print("Peso(kg):");
      //Posiciona o cursor na coluna 0, linha 1;
      lcd.setCursor(0, 1);
      lcd.print(pesoString);
    }
    break;
    
    //Retorna a tela Principal
    case 'C':    
    case 'D':
    posicaoPeso = 0;
    limpaString();
    telaPrincipal();
    break;
    
    //Inseri ponto no peso
    case '.':
    if(posicaoPeso > 0){
      //verifica se ja existe ponto no peso
      bool existePonto = false;
      for(int i = 0; i<=7; i++){
        if(pesoString[i] == '.'){
          existePonto = true;
          break;
        }
      }
      if(existePonto == false){
        pesoString[posicaoPeso] = valor;
        posicaoPeso++;
        //Limpa a tela
        lcd.clear();
        //Posiciona o cursor na coluna 0, linha 0;
        lcd.setCursor(0, 0);
        //Envia o texto entre aspas para o LCD
        lcd.print("Peso(kg):");
        //Posiciona o cursor na coluna 0, linha 1;
        lcd.setCursor(0, 1);
        //Imprime o peso no LCD
        lcd.print(pesoString);
      }
    }
    break;
    
    //adiciona digito no peso
    default:
    pesoString[posicaoPeso] = valor;
    posicaoPeso++;
    //Limpa a tela
    lcd.clear();
    //Posiciona o cursor na coluna 0, linha 0;
    lcd.setCursor(0, 0);
    //Envia o texto entre aspas para o LCD
    lcd.print("Peso(kg):");
    //Posiciona o cursor na coluna 0, linha 1;
    lcd.setCursor(0, 1);
    //Imprime o peso no LCD
    lcd.print(pesoString);
    break;
  }
}

//Insere digito na altura
void alturaInsere(char valor){
  switch(valor){
    
    //Enter
    case 'A':
    if(posicaoAltura > 0){
      posicaoAltura = 0;
      telaIMC();
    }
    break;
    
    //Apaga
    case 'B':
    if(posicaoAltura > 0){
      posicaoAltura--;
      alturaString[posicaoAltura] = '\0';
      //Limpa a tela
      lcd.clear();
      //Posiciona o cursor na coluna 0, linha 0;
      lcd.setCursor(0, 0);
      //Envia o texto entre aspas para o LCD
      lcd.print("Altura(cm):");
      //Posiciona o cursor na coluna 0, linha 1;
      lcd.setCursor(0, 1);
      lcd.print(alturaString);
    }
    break;
    
    //Retorna para Tela Principal
    case 'C': 
    case 'D': 
    posicaoAltura = 0;
    limpaString();
    telaPrincipal();
    break;
    
    case '.':
    break;
    
    //Adiciona o digito na altura
    default:
    alturaString[posicaoAltura] = valor;
    posicaoAltura++;
    //Limpa a tela
    lcd.clear();
    //Posiciona o cursor na coluna 0, linha 0;
    lcd.setCursor(0, 0);
    //Envia o texto entre aspas para o LCD
    lcd.print("Altura(cm):");
    //Posiciona o cursor na coluna 0, linha 1;
    lcd.setCursor(0, 1);
    //Imprime o peso no LCD
    lcd.print(alturaString);
    break;
  }
}