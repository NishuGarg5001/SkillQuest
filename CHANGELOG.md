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