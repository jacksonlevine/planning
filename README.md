# Name TBD / JacksonGame

## Prototype of a React app that uses a FireBase backend and Three.js graphics

## Goals / Description:

* This will be a 3D game that is massively multiplayer, all players actively updating via a FireBase realtime database. Game rendering will be done using Three.js, players will be able to move around and see each other in a 3D space.

* Using FireBase's additional functionality of signing in with a provider such as Google.

* All of this put into a React app to allow for a modular and dynamic user experience.

## Technologies Used:

* React
* Three
* FireBase / Realtime Database

## Installation / Setup:

* Clone this repository
* Navigate to the /planning/project directory
* Run `npm install` inside of /planning/project
* Sign up with FireBase and create a project on their website.
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
* Entry to realtime database is created upon client connecting, and deleted upon client disconnecting!


## Research & Planning Log:

#### Friday, 04/28/2023

* 8:50 Research FireBase
* 9:30 Creating FireBase project in-browser
* 9:50 Researching Supabase as potential other choice
* 10:00 Setting up FireBase
* 1:00 - 2:00 Researching authentication.
* 4:30 - 5:00 Researching realtime database limits and usage.

#### Friday, 05/05/2023

* 8:00 Researching Google auth
* 9:20 Still trying to Google auth
* Google auth!
* 9:30 Research Github authentication with firebase,
to confirm if Google is the issue
* 2:00 - 2:30 Researching PayPal SDK

## License:

Copyright Jackson Levine 2023

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

