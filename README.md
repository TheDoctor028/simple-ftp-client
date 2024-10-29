# Házi feladat

Név/Nevek NEPTUN kóddal:
- Hetényi Kristóf (******)

# Feladat kiírás
A példafeladatok közül választottam az FTP kliens alaklmazást.
A következő követleményekkel:
* Szöveges felhasználó felület
* Belépés az FTP szerverre
* Könyvtárak váltása, listázása
* Állományok letöltése és feltöltése háttérben

# Megvalósított program
_Milyen funkciókat sikerült megvalósítani? Milyen eltérések vannak a kiíráshoz képest? Hogyan kell bekonfigurálni, elindítani?_

A programban lehetőségünkvan csatlakozni egy FTP szerverhez és bejelentkezni arra. A programban van ``help`` parancs
ennek segítségével láthatjuk milyen parancsokal tudunk interaktálni az ftp szerverrel. Lehetőségünk van fileok le- és feltöltése
közben más parancsokat. Az implementációhoz az FTP RFC-t használatam [https://www.rfc-editor.org/rfc/rfc959](https://www.rfc-editor.org/rfc/rfc959).

### Telepítés

``cmake`` parancs használatával.

**Bemutató videó URL:**
https://youtu.be/bx9o8XnQpdc

# Tapasztalatok
_Milyen tapasztalatokat gyűjtött a feladat elkészítése során? Mi volt egyszerűbb / nehezebb a tervezetnél? Visszatekintve mit csinálna másként? (pár mondatban)_

- Sokat felejtettem a C/C++ tudásomból, tovább tartótt a vissza rázódás mint gondoltam
- Az előző pontból kifojólag hamarabb kellett volna elkezdenem, hogy a rendes beadási határidőre elkészüljek
- RFC-k használata
- A tervezésnél túlgondoltam a háttér letöltés implementációját
