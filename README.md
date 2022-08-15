<p>
    <h1 align="center" style="padding: 15px;"><img src="https://raw.githubusercontent.com/Tavisco/StudentOrganizer/master/Rsc/SORG.png" style="width:50px; margin-bottom: -14px"/> Student Organizer (S.O.R.G.)</h1>
</p>
<p align="start">
    <a alt="Palm OS >= 3.5">
        <img src="https://img.shields.io/badge/Palm%20OS-%3E%3D%203.5-blue" />
    </a>
    <a>
        <img alt="GitHub Repo stars" src="https://img.shields.io/github/stars/Tavisco/StudentOrganizer?style=social">
    </a>
        
</p>

This software for Palm OS is intended to help students of all levels that have class schedules and homeworks to better keep track of those.

The main form tells you right away which is the current class, which will be the next, alongside an indicator that tells you if you have homework due tomorrow.

The classes management supports classes that occur multiple days of the week.


## Screenshots
| Main | Classes | Homeworks | Manage Homework |
| :-:  | :-:   | :-:       | :-:             |
| ![Main](https://raw.githubusercontent.com/Tavisco/StudentOrganizer/master/Rsc/screenshot1.png) | ![Classes](https://raw.githubusercontent.com/Tavisco/StudentOrganizer/master/Rsc/screenshot2.png) | ![Homeworks](https://raw.githubusercontent.com/Tavisco/StudentOrganizer/master/Rsc/screenshot3.png) | ![manage Homework](https://raw.githubusercontent.com/Tavisco/StudentOrganizer/master/Rsc/screenshot4.png) |

## Some background
As the Github description says, this is a recreation in C of a very similar application I've developed using [HB++](http://web.archive.org/web/20090622021514/http://www.handheld-basic.com/) when I was about 13 years old. I got the motivation to re-do it in C just to provoke myself with a new challenge, as I'm professionally NOT a C programmer, and to give back some love to the Palm platform, as programming that application in particular back then was a pretty significant step in settling down on choosing "what I will do when I grow up" :)


That being said, this is my first contact with the C language and with the Palm toolchain. Mistakes were probably made, so proceed with caution, and as always, use it at your own risk, I do not guarantee that this software will work as intended.

If you found an issue, please open one here in this repository that I may (or may not) fix it.

## Downloads
You can find the pre-compiled binaries (the .prc files) on the release section of Github. [Or click here to go right to the latest version.](https://github.com/Tavisco/StudentOrganizer/releases/latest)

## Building
To build this application from source use the toolchain available [here](https://www.palm2000.com/projects/compilingAndBuildingPalmOsAppsOnUbuntu2004LTS.php), edit the paths of the building tools on the makefile, then simply run `make` in the root of the project.

## Special thanks
I'd like to thank all the awesome people on [PalmDB's Discord Server](https://palmdb.net/) that helped me with begginer's issues with C and Palm OS development; that demonstrated interest in the project and pushed me forward to complete it; that sent ideas and opinated about the features and looks of it; and probably much more things. Many thanks to you all!