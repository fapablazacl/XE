param (
    [Parameter(Mandatory=$true)][string]$mode,
    [Parameter(Mandatory=$true)][string]$toolchain,
    [Parameter(Mandatory=$true)][string]$buildType
)

write-output $mode
write-output $toolchain
write-output $buildType
