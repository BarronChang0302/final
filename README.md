# final
# hw4
The structure in the following picture can be used in all 3 parts. <br>
![195981680_472640117332620_8393154622717387120_n](https://user-images.githubusercontent.com/79574115/121262991-8cca4980-c8e7-11eb-9490-8f53aa978ba1.jpg)
![196212873_319378759804997_2960565786612567354_n](https://user-images.githubusercontent.com/79574115/121262995-8f2ca380-c8e7-11eb-8bb1-1d69ffd89e9e.jpg)
![196054281_163966635747804_1284039958029825564_n](https://user-images.githubusercontent.com/79574115/121262997-905dd080-c8e7-11eb-850f-79c597aeca33.jpg)
![196003108_119939950182851_2034282517340157998_n](https://user-images.githubusercontent.com/79574115/121262999-92279400-c8e7-11eb-96bc-51f8957a62fb.jpg)

### Route of car <br>
(a) Overview (see picture) <br>
Yellow: Start -> Follow line -> Follow AprilTag -> Reverse Parking <br>
Red:    Go forward -> Follow Apriltag -> Go forward (using ping) -> Turn right -> Go forward (using ping) -> Turn right -> Go forward (using ping) -> Classify dog image -> Turn right <br>
Blue:   Modify direction -> Follow line -> Follow AprilTag -> Turn Around -> Modify direction -> Follow line -> Go forward (using ping) -> Turn left <br>
Green:  Modify direction -> Go forward (using ping) -> Classify cat image <br>
![199768216_1170523030128235_9157478161956475658_n](https://user-images.githubusercontent.com/79574115/121821856-4f423380-ccce-11eb-8ec1-6f42a53b66db.png)
(b) Dog image <br>
![198791911_834683794124648_129817312226115005_n](https://user-images.githubusercontent.com/79574115/121821869-62ed9a00-ccce-11eb-8563-2e7ea0ef28ac.jpg)
(c) Cat image <br>
![199716254_390398105697708_4342327163689900390_n](https://user-images.githubusercontent.com/79574115/121821882-6f71f280-ccce-11eb-94be-5bbd4dfc203b.jpg)

### Setup and Run <br>
(a) main.cpp for mbed, xbee.py for PC, final.py for OpenMV. <br>
(b) Add optical encoder, ping, OpenMV and XBee to the car(mbed). <br>
(c) The following table show the pins and the corresponding function. <br>
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
