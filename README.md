# Robô Wifi para ESP32

O wifi manager é baseado no código:
https://randomnerdtutorials.com/esp8266-nodemcu-wi-fi-manager-asyncwebserver/ <br>
<br>
O joystick é baseado no código:
https://www.instructables.com/Making-a-Joystick-With-HTML-pure-JavaScript/

## Lista de materiais
- ESP32
- Ponte H (arduino)
- Conectores macho-fêmea de protoboard (pelo menos 6)
- Fios diversos
- 1 Protoboard pequena
- 2 Bateria 9V com conectores apropriados
- 1 regulador de tensão <a href="images/regulador.png">L7805C</a>
- Chassi de carrinho arduino
    - 2 motores DC
    - Caixa redutora
    - 2 Rodinhas
    - Chassi de acrílico
    - Chave gangorra

## Pinagem
(Ponte H) = (ESP32)
- IN1 = D15
- IN2 = D4
- IN3 = D18
- IN4 = D19

Os demais pinos são alimentação

## Esquemático
<img src="images/Esquema.png">

## Robô montado
<img src="images/Montado.jpeg">

## Configuração do Software
- Abra o código utilizando o <a href="https://platformio.org/">PlatformIO</a>, conecte a ESP32 no computador
e carregue o código.
- Desconecte o cabo USB e conecte a energia da bateria
- Procure pela rede ESP-WIFI-MANAGER em seu smarphone 
- Conecte ao endereço ip padrão (192.168.4.1)
- Realize as configurações do WiFi