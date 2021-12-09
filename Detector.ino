#include <MPU6050_tockn.h> //Biblioteca para facilitar a leitura de dados
#include <Wire.h> //Biblioteca para a comunicação I2C


MPU6050 mpu6050(Wire); //Entrega a biblioteca Wire.h como argumento para a biblioteca MPU6050_tockn.h
//z está aumentando muito com o tempo

float Grau_x = 0, Grau_y = 0, grau_padrao_parado[2], grau_atual_esquerda[2];// Declaração de variáveis
int validador = 0;
/*
Validador = 0 modo de calibração
Validador = 1 parado
validador = 2 movimento detectado
*/
const float K = 100.25; //sensibilidade do controle, caso esteja muito díficil de fazer se movimentar é só abaixar a sensisibilidade, caso se movimente muito fácilmente é só aumentar a sensibilidade

void setup() {
  Serial.begin(9600); //Inicializa a comunicação serial a uma taxa de 9600 bit por segundo


  Wire.begin(); //Inicializa a biblioteca Wire.h
  
  mpu6050.begin(); //Inicializa a biblioteca MPU6050_tockn.h
  mpu6050.calcGyroOffsets(true); //Ativa a calibração interna da biblioteca
  Serial.println();
  
}
void loop() {
  
  if (validador == 0){
    
    calibrador();  
    validador = 1;
  
  }

  leitor();
  detector_de_movimento();
  
  if (validador == 2){

    detector_de_direcao();
  
  }

}

//função que lê os dados do mpu
void leitor(){
  
  int count = 0;

  Grau_x = 0;
  Grau_y = 0;

  while(count < 100){

    mpu6050.update(); //atualiza a leitura

    //Obtém os angulos
  
    Grau_x = Grau_x + mpu6050.getAngleX();
    Grau_y = Grau_y + mpu6050.getAngleY();
    count++;
  }

  //Média dos angulos medidos
  Grau_x = Grau_x/100;
  Grau_y = Grau_y/100;

}

//função que define qual a posição de parado
void padrao_parado(){
  
  int count = 0;
 
  grau_padrao_parado[0] = 0;
  grau_padrao_parado[1] = 0;
  
  while (count < 10){
  
    leitor();
    grau_padrao_parado[0] += Grau_x;
    grau_padrao_parado[1] += Grau_y;
    count++;
  
  }
  
  grau_padrao_parado[0] /= 10;
  grau_padrao_parado[1] /= 10;
  
}

//função que converte os dados do MPU em uma direção
void detector_de_movimento(){

  float aux[2];


  //
  if ( (grau_padrao_parado[0]*grau_padrao_parado[0]) > (Grau_x*Grau_x) ){

    aux[0] = (grau_padrao_parado[0]*grau_padrao_parado[0]) - (Grau_x*Grau_x);

  }
  
  else{

    aux[0] = (Grau_x*Grau_x) - (grau_padrao_parado[0]*grau_padrao_parado[0]);

  }

  if( (grau_padrao_parado[1]*grau_padrao_parado[1]) > (Grau_y*Grau_y) ){

    aux[1] = (grau_padrao_parado[1]*grau_padrao_parado[1]) - (Grau_y*Grau_y);
  
  }

  else{

    aux[1] = (Grau_y*Grau_y) - (grau_padrao_parado[1]*grau_padrao_parado[1]);
  
  }

  if ( (aux[1] > K) && (aux[0] > K) ){

    validador = 2;
    
  }

  else{

    validador = 1;
     
  }
  
}

void detector_de_direcao(){

  if ( (grau_padrao_parado[0] > Grau_x) && (grau_padrao_parado[1] < Grau_y) ){

    Serial.println("Direita");

  }

  else if ( (grau_padrao_parado[0] > Grau_x) && (grau_padrao_parado[1] > Grau_y) ){

    Serial.println("Frente");

  }

  else if ( (grau_padrao_parado[0] < Grau_x) && (grau_padrao_parado[1] < Grau_y) ){

    Serial.println("Tras");

  }

  else if ( (grau_padrao_parado[0] < Grau_x) && (grau_padrao_parado[1] > Grau_y) ){

    Serial.println("Esquerda");

  }

}

//função que calibra o sensor para o código funcionar em qualque mpu
void calibrador(){

    Serial.println("Iniciando a calibração aguarde parado");
    delay(6000);
    padrao_parado();
    Serial.println("Sensor calibrado e pronto para uso");
    delay(3000);
    
}
