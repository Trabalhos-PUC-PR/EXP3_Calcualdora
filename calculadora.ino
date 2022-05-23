#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'C','0','=','/'}
};

enum{
  DECIMAL = 10,
  OCTAL = 8,
  BINARY = 2
} state = DECIMAL;

bool isCalculator = true;

char operators;
String parameter1;
String parameter2;
int decimalParameter1;
int decimalParameter2;
long result;

byte rowPins[ROWS] = {A0,A1,A2,A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A4,A5,8,9}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

const int 
  rs = 12, 
en = 11, 
d4 = 5, 
d5 = 4, 
d6 = 3, 
d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int cursor = 0;

void setup(){  
  lcd.begin(16, 2);
  Serial.begin(9600);
}

void loop(){
  printHeader();
  calcOperations();
}

void changeState(){
  switch(state){
    case DECIMAL:  state=OCTAL; break;
    case OCTAL:   state=BINARY; break;
    case BINARY: state=DECIMAL; break;
  }
  if(state == DECIMAL){
    isCalculator = !isCalculator;
  }
  clearLdc(false);
}

void calcOperations(){
  char key = customKeypad.getKey();
  switch(key){
    case '0' ... '9':
    bool isFirstParam;
    isFirstParam = false;
    if(operators == 0){
      isFirstParam = true;
    }
    setParameter(key, isFirstParam);
    break;
    case '+':
    case '-':
    case '/':
    case '*':
    setOperator(key);
    break;  
    case '=':
    result = calculateParams();
    printLcd('=');
    printLcd(result);
    resetAll();
    break;  
    case 'C':
    resetAll();
    clearLdc(true);
    break;
  }
}

void clearLdc(bool lower){
  cursor = 0;
  lcd.setCursor(cursor, lower);
  lcd.print("                ");
  cursor = 0;
  lcd.setCursor(cursor, lower);
}

void printLcd(long result){
  lcd.setCursor(cursor, 1);
  lcd.print(result);
  cursor++;
}

void printLcd(char term){
  lcd.setCursor(cursor, 1);
  lcd.print(term);
  cursor++;
}

void printHeader(){
  lcd.setCursor(0, 0);
  if(isCalculator){
    lcd.print("calc");
  }else{
    lcd.print("conv");
  }
  lcd.print("          ");
  lcd.print(state);
  lcd.setCursor(cursor, 1);
}

long calculateParams(){
  if(parameter2 == "" && parameter1 == ""){
    changeState();
    clearLdc(true);
    return 0;
  }
  long result = 0;
  if(isCalculator){
    if(parameter2 == "" && parameter1 != ""){
      return parameter1.toInt();
    }
    switch(operators){
      case '+':
      Serial.println(convert(parameter1, 10));
      Serial.println(convert(parameter2, 10));
      result = convert(parameter1, 10) + convert(parameter2, 10); 
      break;
      case '-':
      result = convert(parameter1, 10) + convert(parameter2, 10); 
      break;
      case '/':
      result = convert(parameter1, 10) + convert(parameter2, 10); 
      break;
      case '*':
      result = convert(parameter1, 10) + convert(parameter2, 10); 
      break;
    } 
    result = convertFromDecimal(String(result), state);
  }else{
    return convertFromDecimal(parameter1, state);
  }
  return result;
}

long convertFromDecimal(String str, int base){
  long number = str.toInt();
  String result = "";

  while(number > 0){
    result = (number % base) + result;
    number /= base;
  }
  return result.toInt();
}

long convert(String str, int base){
  if(state == DECIMAL){
    return str.toInt();
  }
  
  int size = str.length();
  int cont = 0;
  long result = 0;
  int expo = size-1;
  
  while(cont != size){
    String toBeNumber = String(str.charAt(cont));
    long number = toBeNumber.toInt();
	result += base*pow(number*base, expo);
    expo--;
    cont++;
  }

  return result;
}

int pow(int base, int expo){
  int result = base;
  if(expo == 1)return base;
  else if(expo == 0) return 1;
    while(expo > 1){
    result = result*base;
    expo--;
  }
  return result;
}

void resetAll(){
  operators = 0;
  parameter1 = "";
  parameter2 = "";
}

void setParameter(char key, bool firstParam){
  if(parameter1==""){
    clearLdc(true);
  }
  if(firstParam){
    parameter1+=key;
  }else{
    parameter2+=key;
  }
  printLcd(key);
}

void setOperator(char key){
  if(parameter1!=""){
    operators = key;
    printLcd(key);	
  }
}