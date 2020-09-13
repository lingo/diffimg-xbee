# only for macx target
mkdir -p Release/diffimg.app/Contents/translations
cp ../lang/*.qm Release/diffimg.app/Contents/translations
cp ../CREDITS.txt ../README.txt ../AUTHORS.txt ../Changelog.txt Release/diffimg.app/Contents/Resources
rm Release/*.dmg 2> /dev/null
macdeployqt Release/diffimg.app -dmg
