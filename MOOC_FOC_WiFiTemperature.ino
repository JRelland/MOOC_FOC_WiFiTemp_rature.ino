/*                Programme ESP8266
 *          MOOC_FOC_WiFiTemp_rature.ino
 *             José Relland - Juin 2016
 *           troyesaubefablab@gmail.com
 *               Domaine Public
 *  
 *           Inpiré par le programme 
 *            WiFiTemperature.ino 
 *    de Laurent Toutain (Télécom Bretagne) 
 *     du MOOC "Fabriquer un objet connecté"
 *       Institut Mines-Télécom (Bretagne)
 *           https://goo.gl/53LD9H
 *  
 */

#include <ESP8266WiFi.h>

// valeurs pour le WiFi
const char* ssid     = " - Votre Wifi- ";                   // Entrer le nom de votre Wifi (SSID)
const char* password = " - Votre mot de passe Wifi - ";     // Entrer le mot de passe de votre Wifi

// valeurs pour le serveur Web
const char* host     = "api.openweathermap.org";            // Le site où vous devez ouvrir un compte 
                                                            // et demander une clé API (gratuit)
const char* apikey   = " - Votre clé API -";  // MyAPI      // Votre clé API de la forme : a6e54gc....
const char* town     = "Troyes,fr";                         // Choisir la ville, mettre le code pays ISO sur deux caractères

String keyword = String("\"temp\":"); //chaîne "temp" (température) que l'on recherche dans le JSON

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.print("Connexion au WiFi ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);    // On se connecte
  
  while (WiFi.status() != WL_CONNECTED) { // On attend
    delay(500);
    Serial.print(".");
  }

  Serial.println("");  // on affiche les paramètres 
  Serial.print("WiFi connect") ; Serial.write(233) ;  Serial.println("");  
  Serial.print("Adresse IP du module EPC: ");  
  Serial.println(WiFi.localIP());
  Serial.print("Adresse IP de la box : ");
  Serial.println(WiFi.gatewayIP());
}


// drapeau indiquant pendant l'analyse de la réponse du serveur
// si on est dans l'en-tête HTTP (false) ou dans le contenu de 
// la ressource.
bool inBody = false;


void loop() {
  float temperature = 0; /* lue du fichier JSON reçu */

  Serial.print("Connexion au serveur : ");
  Serial.println(host);
  
  // On se place dans le rôle du  client en utilisant WifiClient
  WiFiClient client;

  // le serveur Web attend tradionnellement sur le port 80
  const int httpPort = 80;

  // Si la connexio échoue cela sera pour la prochaine fois
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  
  // La connexion a réussie on forme le chemin 
  // URL  complexe composé du chemin et de deux 
  // questions contenant le nom de ville et l'API key
  
  String url = String("/data/2.5/weather?q=") + town + "&appid=" + apikey;
  
  Serial.print("demande URL: ");
  Serial.println(url);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  // On attend 1 seconde
  delay(10000);
  
  inBody = false; // on est dans l'en-tête
  
  // On lit les données reçues, s'il y en a
  while(client.available()){
    String line = client.readStringUntil('\r');
        
    if (line.length() == 1) inBody = true; /* passer l'en-tête jusqu'à une ligne vide
                                               avec un caractère de retour ligne*/
    if (inBody) {  // ligne du corps du message, on cherche le mot clé
       int pos = line.indexOf(keyword);   // Permet de trouver la position de "temp" (température)

      if (pos > 0) { /* mot clé trouvé */
       // indexOf donne la position du début du mot clé, en ajoutant sa longueur
       // on se place à la fin.
       pos += keyword.length(); 

       Serial.println (&line[pos]);

       temperature = atof(&line[pos]); 

     } /* fin récupération du flotant */
    } /* fin de la recherche du mot clé */
  } /* fin data avalaible */
  
  Serial.println() ; 
  Serial.print("Temp") ; Serial.write(233) ;  Serial.print("rature = "); Serial.print(temperature-273.15); Serial.write(186) ; // temp en Kelvin
  Serial.println() ;
  
  Serial.print("connexion ferm") ; Serial.write(233) ;  Serial.println("e");

  Serial.println() ; Serial.print("******************************") ; Serial.println() ;

  delay(120000);      // ici, la fréquence de rafraîchissement est 2 minutes
                      // Vous pouvez changer la valeur (en millisecondes)
                      
}
