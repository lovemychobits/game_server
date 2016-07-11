@set Path=%cd%
@set ExePath=%Path%/proto-net-gen/
@set ProtoPath=%Path%/
@set CsPath=%Path%/cs/

@for %%i in (%ProtoPath%*) do (
  @%ExePath%protogen.exe -i:%ProtoPath%%%~ni.proto  -o:%CsPath%%%~ni.cs
  @echo %%~ni.proto Complete
)

@echo All complete
@pause()