.\protoc.exe login.proto --cpp_out=../
.\protoc.exe slither_client.proto --cpp_out=../


.\protoc.exe slither_client.proto --python_out=../../python-scripts

pause()