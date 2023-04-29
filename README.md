# Name TBD / JacksonGame

## Prototype of a React app that uses a Firebase backend and Three.js graphics

## Goals:

* This will be a 3D game that is massively multiplayer, all players actively updating via a Firebase realtime database. All 3D rendering will be done using Three.js, so players will be able to move around and see each other in 

## Installation / Setup:

* Clone this repository
* Navigate to the /planning/project directory
* Run `npm install` inside of /planning/project
* Sign up with Firebase and create a project on their website.
* Navigate on the left panel to "Build" and then "Realtime Database" (Note: if you do not see "Build", click on the empty space of the leftmost black panel. The menu will expand.)
* Click "Create Database"
* After creating, in the "Rules" tab, paste the following code, which will allow the correct permissions for this project:
```
{
  "rules": {
    ".read": "auth != null",
    ".write": false,
    "players": {
      "$uid": {
        ".write": "auth != null && auth.uid == $uid"
      }
    }
  }
}
```
* Under Build -> Authentication, on the "Sign-in method" tab, make sure to allow the provider "Anonymous".
* Run `npm run start` inside of /planning/project
* Entry to realtime database is created upon client connecting, and deleted upon client disconnecting. 


## Research & Planning Log:

#### Friday, 04/28/2023

* 8:50 Research Firebase
* 9:30 Creating Firebase project in-browser
* 9:50 Researching Supabase as potential other choice
* 10:00 Setting up Firebase
* 1:00 - 2:00 Researching authentication.
* 4:30 - 5:00 Researching realtime database limits and usage.