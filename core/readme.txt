# Little readme:

Description:
Application to run neurofeedback experiments. Currently created sessions:
-Gamma
-Theta/Alfa
-> Suitable for meditative sessions.

Supported devices currently:
-NeuroSky ThinkGear. Tested with MindWave.
-OpenEEG. Tested with Olimex EEG-SMT.

#Running app:
1. Set mode from F2 / F3 of the session or change configuration more precisely in nfb-panel.
2. Make sure EEG-device is working properly. You should see nice eeg graphs. Change configurations in settings-panel for device and reboot application If neccessary.
3. Put EEG-device on head and earplugs.
4. Press play. Session will quit automatically when it's done.

*Results of each session will be saved to results-folder.
*Autosaving during sessions. It will continue from the point where you quit during session.
*Configurations will be saved and loaded when closing and opening app automatically.

If app tells that open eeg device doesn't give packets and you have to restart:
1. Just reboot and continue.

#Compile requirements:
-easy to just use code blocks to open the actual project file.
-requires c++ compiler mingw or similar to compile. 
-allegro lib
-audiere lib
-fftw lib

#Desired coming features:
-Mapping for profile's repeated sessions for progress monitoring.

Feel free to use the app for any purpose.

Tonis 2014