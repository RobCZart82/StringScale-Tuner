; Build only after package_preview.py and verify_preview.py have succeeded.
#ifndef StageDir
  #error StageDir is required
#endif
#ifndef OutputDir
  #error OutputDir is required
#endif
#ifndef AppVersion
  #error AppVersion is required
#endif
#ifndef NumericVersion
  #error NumericVersion is required
#endif
#ifndef SetupName
  #error SetupName is required
#endif

[Setup]
AppId=org.robczart.stringscaletuner
AppName=StringScale Tuner
AppVersion={#AppVersion}
AppPublisher=RobCZart82
AppPublisherURL=https://github.com/RobCZart82/StringScale-Tuner
AppSupportURL=https://github.com/RobCZart82/StringScale-Tuner/issues
DefaultDirName={localappdata}\Programs\StringScaleTuner
DefaultGroupName=StringScale Tuner
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0.17763
OutputDir={#OutputDir}
OutputBaseFilename={#SetupName}
VersionInfoVersion={#NumericVersion}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayIcon={app}\bin\StringScaleTuner.exe
CloseApplications=yes
RestartApplications=no
LicenseFile={#StageDir}\LICENSE

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#StageDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\StringScale Tuner"; Filename: "{app}\bin\StringScaleTuner.exe"; WorkingDir: "{app}\bin"
Name: "{group}\User manual (English)"; Filename: "{app}\manuals\StringScale-Tuner-User-Manual-EN.pdf"
Name: "{group}\Felhasznaloi kezikonyv (Magyar)"; Filename: "{app}\manuals\StringScale-Tuner-User-Manual-HU.pdf"
Name: "{autodesktop}\StringScale Tuner"; Filename: "{app}\bin\StringScaleTuner.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\bin\StringScaleTuner.exe"; Description: "{cm:LaunchProgram,StringScale Tuner}"; Flags: nowait postinstall skipifsilent
