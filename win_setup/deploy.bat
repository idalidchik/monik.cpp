call setupenv.bat

mkdir %BUILD_DIR%

rem force reinstall every time by recreating deploy directory
del /S /Q %DEPLOY_DIR%
rmdir /s /q %DEPLOY_DIR%

mkdir %DEPLOY_DIR_BASE%
mkdir %DEPLOY_DIR%

cd %SCRIPTS_DIR%
call build_protobuf.bat
call build_openssl.bat
call build_rabbitmq-c.bat

cd %SCRIPTS_DIR%
call monik_build.bat

