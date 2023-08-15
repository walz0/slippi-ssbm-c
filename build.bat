@REM "m-ex/MexTK/MexTK.exe" -ff -i "Scenes/Ranked/GameSetup.c" "Components/CharStageBoxSelector.c" "Components/CharStageIcon.c" "Components/Button.c" "Components/FlatTexture.c" "Components/RightArrow.c" "Components/CharPickerDialog.c" "Components/StockIcon.c" "Components/GameResult.c" "Components/TurnIndicator.c" "Game/Characters.c" ^
@REM -s mnFunction ^
@REM -o "output/GameSetup.dat" ^
@REM -t "m-ex/MexTK/mnFunction.txt" ^
@REM -q -ow -c -l "melee.link"

"m-ex/MexTK/MexTK.exe" -ff -i "Scenes/Ranked/RstScreen.c" ^
-s mnFunction ^
-o "output/GameSetup.dat" ^
-t "m-ex/MexTK/mnFunction.txt" ^
-q -ow -c -l "melee.link"

"m-ex/MexTK/MexTK.exe" -ff -i "Scenes/CSS/SlippiCSSSetup.c" ^
-s mnFunction ^
-o "output/SlippiCSS.dat" ^
-t "m-ex/MexTK/mnFunction.txt" ^
-q -ow -c -l "melee.link"

@REM mv "C:\Users\walz\slippi-ssbm-c\output\GameSetup.dat" "C:\Users\walz\ishiiruka\Binary\x64\Sys\GameFiles\GALE01\GameSetup.dat"
@REM mv "C:\Users\walz\slippi-ssbm-c\output\SlippiCSS.dat" "C:\Users\walz\ishiiruka\Binary\x64\Sys\GameFiles\GALE01\SlippiCSS.dat"