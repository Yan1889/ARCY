$TAR_DIR = "build_web(dezip)"
Remove-Item -Path "$TAR_DIR\*" -Recurse

Copy-Item -Path "build_web/ARCY.*" -Destination "$TAR_DIR\"
Copy-Item -Path build_web/assets -Destination "$TAR_DIR\"
Copy-Item -Path index.html -Destination "$TAR_DIR\"
