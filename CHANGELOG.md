#V 0.03831
-Added mouse input, functionality to mine ores using mouse click
-Fixed sprites to use nearest neighbour interpolation
-Todo: icons to switch screen states, re-implement vault with deposit and withdraw functionality, toolbelt

#V 0.03830
-Removed some features such as vault and foraging to streamline transition to textual to mouse/keyboard input
-Simplified the code by a lot by removing all textual input handling
-Removed skills, levels and experience for now to simplify gameplay, drop rates and rarities have been kept
-Simplified Game class by splitting into multiple Screen classes, with each Screen class taking its role and Game class
orchestrating the roles
-Added resource sprites
-Added resource table
-Todo: Implement mouse input, re-implement vault with deposit and withdraw functionality, implement toolbelt and icons

#V 0.03826
-Critical crash fix
-Cleaned verbose and limited to 5 lines of text

#V 0.0.3825
-Added vault to store items
-Added deposit command to deposit items to vault
-Sprites added for tin, iron, gold ore and stone pickaxe
-Todo: implement withdraw command, add toolbelt functionality

#V 0.0.3820
-Cleaned code, got rid of enums as led to unnecessary parsing, textual commands now directly map to actions
-Added Foraging skill
-Added new resource "ground" accessed via "search ground"
-Added new objects "stone" and "stick"
-Todo: sprites for tin, iron and gold ores, add toolbelt functionality, add bank
-Added a list of controls and valid commands to README.md

#V 0.0.3815
-Added graphics to inventory
-Added sprite for copper ore
-Added support to view inventory and mining progress simultaneously
-Set font size to scale with SCREEN_HEIGHT
-Updated README.md to include SDL3_image
-Todo: more sprites, Foraging skill and toolbelt

#V 0.0.381
-Added scalable UI for inventory supporting 8x8 px, 16x16 px, 32x32 px and 64x64 px items

#V 0.0.380
-Project shifted from terminal to SDL3
-Updated README.md

#V 0.0.373
-Combined a lot of maps together
-Shifted most of the current game data into Resource class attributes (where it should actually belong)
-Expanded Resource class to store a vector of objects instead of just one object
-Using enum namespaces now, simplified code
-Todo: Add functionality to view inventory --> will require generalizing command parser

#V 0.0.372
-Added drop rate and rarity attributes to resources
-Added drop rate to rarity mapping
-Simplified Object struct
-Added more mappings
-Added DropResult struct which is result of Object generation
-Added rarity based color coding to verbose
-Added implementation for tracking exp progress
-Added support for Unicode characters
-enableansi.h renamed to consolesettings.h
-Todo: Add functionality to view inventory --> will require generalizing command parser

#V 0.0.371
-Converted all enums to enum classes
-Separated InputKey to CommandKey and TextualKey
-Converted most enum->enum/string maps from const expr arrays to switch case functions
-Converted xp table to a switch case function
-Updated pushHistory() logic to check for texts longer than COMMAND SCREEN WITH
-Converted all size like constants and variables to size_t
-Added .vscode/ to .gitignore

#V 0.0.370
-Added experience gain
-Added level up
-Added colors for ores
-Removed location, game will only have 1 location for now
-Cleaned up resource and object classes
-Cleaned up player skills as a hash map (skills enum -> (level, exp)) instead of individual ints
-Exp gain via xp tables (hash maps)
-Added resource extraction based on random number gen rate
-Player resource target is now correctly owned by game
-Added function to check real length of history strings (vs. virtual length including ANSI characters)
-Added mapping from skill-type action to skill name and vice versa to pull out general code from switch-case

#V 0.0.365
-Optimised history from a vector to fixed sized deque (tied to SCREEN_HEIGHT - 1) by popping from front and pushing from back
-Optimised frame_buffer to fixed size array (tied to SCREEN_HEIGHT)
-Memory optimizations, constants size reduced to uint8_t, removed an unused member function in Menu class
-Added support for other Windows terminals

#V 0.0.360
-File split into main and multiple header files

#V 0.0.350
-Added functionality to validate user input
-Added inventory
-Added basic support for mining skill
-Added multiple enum categories
-Added Location, Objects and Resources
-Cleared up some strings with string_views
-Added tick size to decouple game state from frame rate
-Added unordered maps to map user input to enums
-Added basic verbose for mining / inventory full
-Added player location and target pointers
-Added various methods to player and game to handle new changes

#V 0.0.270
-Centered menus and boxed them
-Command echoing shifted to last row of screen (not necessarily last row of terminal)

#V 0.0.240
-Optimised enums and player skills to consume less memory
-Fixed some bugs regarding clearing frame
-Made the Menu class water tight by clearly stating and working out the invariant conditions

#V 0.0.200
-Removed Input class as not useful
-Screen width/height changed to size_t
-Added functionality to type and delete commands with echo
-Added command history
-Added functionality to render last screen_height - 1 command histories
-New game is now properly called with its own method, resetting various state variables

#V 0.0.120
-Added .exe files to .gitignore
-Added Enum class for keystrokes
-Changed some elif ladders to switch cases
-Added Input class, cursor options moved to Input class

#V 0.0.100
-Added Changelog
-Game arguments shifted to default constructor, main code cleaned
-renderMenu() optimized