# final
The structure in the following picture can be used in all 3 parts. <br>
![195981680_472640117332620_8393154622717387120_n](https://user-images.githubusercontent.com/79574115/121262991-8cca4980-c8e7-11eb-9490-8f53aa978ba1.jpg)
![196212873_319378759804997_2960565786612567354_n](https://user-images.githubusercontent.com/79574115/121262995-8f2ca380-c8e7-11eb-8bb1-1d69ffd89e9e.jpg)
![196054281_163966635747804_1284039958029825564_n](https://user-images.githubusercontent.com/79574115/121262997-905dd080-c8e7-11eb-850f-79c597aeca33.jpg)
![196003108_119939950182851_2034282517340157998_n](https://user-images.githubusercontent.com/79574115/121262999-92279400-c8e7-11eb-96bc-51f8957a62fb.jpg)

### Route of car <br>
(a) Overview (see picture) <br>
Task1 (yellow): Start -> Follow line -> Follow AprilTag -> Reverse Parking <br>
Task2 (red)   :    Go forward -> Follow Apriltag -> Go forward (using ping) -> Turn right -> Go forward (using ping) -> Turn right -> Go forward (using ping) -> Classify dog image -> Turn right <br>
Task3 (blue):   Modify direction -> Follow line -> Follow AprilTag -> Turn Around -> Modify direction -> Follow line -> Go forward (using ping) -> Turn left <br>
Task4 (green):  Modify direction -> Go forward (using ping) -> Classify cat image <br>
![199768216_1170523030128235_9157478161956475658_n](https://user-images.githubusercontent.com/79574115/121821856-4f423380-ccce-11eb-8ec1-6f42a53b66db.png)
(b) Dog image <br>
![198791911_834683794124648_129817312226115005_n](https://user-images.githubusercontent.com/79574115/121821869-62ed9a00-ccce-11eb-8563-2e7ea0ef28ac.jpg)
(c) Cat image <br>
![199716254_390398105697708_4342327163689900390_n](https://user-images.githubusercontent.com/79574115/121821882-6f71f280-ccce-11eb-94be-5bbd4dfc203b.jpg)

### Setup and Run <br>
(a) main.cpp for mbed, xbee.py for PC, final.py for OpenMV. <br>
(b) labels.txt and trained.tflite also for OpenMV. <br>
(c) Add optical encoder, ping, OpenMV and XBee to the car(mbed). <br>
(d) The following table show the pins and the corresponding function. <br>
| Function | Pin |
|  ----  | ----  |
| Left servo| D5 |
| Right servo | D6 |
| Optical encoder | D11 |
| XBee(rx) | D9 |
| XBee(tx) | D10 | 
| OpenMV(rx) | D0 in mbed, P4 in OpenMV|
| OpenMV(tx) | D1 in mbed, P5 in OpenMV| 
| Ping | D12 |

(e) Execute xbee.py first, it will show the message of results and when every subtask is end. <br>
(f) Compile main.cpp to mbed then the car will start. <br>
(g) Since the room is narrow, I can't put too much line or tag, and the servo is not ideal. I have to modify the direction in some critical points. xbee.py will ask you "Enter:v" when there is a need of modification. Type 1 for turn right, 2 for turn left, 4 for turn left around, 5 for turn right around, 3 for skip. <br>

### Result <br>
(a) Finish line detection <br>
![197622506_196982722301421_8944878711890183443_n](https://user-images.githubusercontent.com/79574115/121822368-66ceeb80-ccd1-11eb-95eb-18f5c982b4b3.png)
![198432840_300843398411911_812641380333253358_n](https://user-images.githubusercontent.com/79574115/121822401-88c86e00-ccd1-11eb-8ffb-64f23b00f7c4.png)

(b) Finish Apriltag calibration <br>
![198987959_1846967682139119_7905934000229943626_n](https://user-images.githubusercontent.com/79574115/121822416-92ea6c80-ccd1-11eb-84e0-cfbd7c057602.png)
![197781794_864401294434132_6633810895325272014_n](https://user-images.githubusercontent.com/79574115/121822418-94b43000-ccd1-11eb-98f8-eca1c16f763a.png)

(c) Finish reverse parking <br>
![198038332_195609685779184_2843686435674844709_n](https://user-images.githubusercontent.com/79574115/121822430-a5fd3c80-ccd1-11eb-8f05-f060ac5b9b7b.png)
![198252220_814648239450248_6116845894913134614_n](https://user-images.githubusercontent.com/79574115/121822435-a7c70000-ccd1-11eb-8523-024987a09298.png)

(d) Classify dog image <br>
![197998733_140655018041435_3548819678452830030_n](https://user-images.githubusercontent.com/79574115/121822450-ba413980-ccd1-11eb-805d-6e77d737b42e.png)
![198529192_194809325854714_4306414073535663435_n](https://user-images.githubusercontent.com/79574115/121822453-bc0afd00-ccd1-11eb-94c5-467baa4be758.png)

(e) Turn around <br>
![200341589_321693199514917_978829107412406380_n](https://user-images.githubusercontent.com/79574115/121822467-cdeca000-ccd1-11eb-80fb-cc1ead6959f0.png)

(f) Need modification <br>
![199245128_1935905643230342_7164992300410400248_n (2)](https://user-images.githubusercontent.com/79574115/121822489-ed83c880-ccd1-11eb-9ea7-45c0f7a6aaaa.png)

(d) Classify cat image <br>
![198543394_197402718818255_1143969679462218930_n](https://user-images.githubusercontent.com/79574115/121822500-faa0b780-ccd1-11eb-87ef-92d20d3e0c9b.png)
![199768343_324440675813145_460815597352690438_n](https://user-images.githubusercontent.com/79574115/121822501-fbd1e480-ccd1-11eb-906a-743e85445bb7.png)
