# Robô Wifi para ESP32

O wifi manager é baseado no código:
https://randomnerdtutorials.com/esp8266-nodemcu-wi-fi-manager-asyncwebserver/ <br>
<br>
O joystick é baseado no código:
https://www.instructables.com/Making-a-Joystick-With-HTML-pure-JavaScript/

## Lista de materiais
- ESP32
- Ponte H (arduino)
- Conectores fêmea-fêmea de protoboard (pelo menos 6)
- Bateria 9V
- Chassi de carrinho arduino
    - 2 motores DC
    - Caixa redutora
    - 2 Rodinhas
    - Chassi de acrílico
    - Chave gangorra

## Esquemático
<img src="Esquema.png">

## Robô montado
<img src="Montado.jpeg">

## Configuração do Software
- Abra o código utilizando o <a href="https://platformio.org/">PlatformIO</a>, conecte a ESP32 no computador
e carregue o código.
- Desconecte o cabo USB e conecte a energia da bateria
- Procure pela rede ESP-WIFI-MANAGER em seu smarphone 
- Conecte ao endereço ip padrão (192.168.4.1)
- Realize as configurações do WiFi