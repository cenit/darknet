#!/usr/bin/env pwsh

#1) build darknet
.\build.ps1
Copy-Item .\build_win_release\Release\*.dll .

#preprocess_flir_dataset.sh
$FLIR_PREFIX = "D:\stefano\Downloads\FLIR_ADAS"
$train_images = "$FLIR_PREFIX/training/PreviewData"
$validation_images = "$FLIR_PREFIX/validation/PreviewData"
$video_images = "$FLIR_PREFIX/video/PreviewData"
$train_anns = "$FLIR_PREFIX/training/Annotations"
$validation_anns = "$FLIR_PREFIX/validation/Annotations"
$video_anns = "$FLIR_PREFIX/video/Annotations"
$train_file = "$PWD/thermal_train.txt"
$valid_file = "$PWD/thermal_validation.txt"
$cfg_file = "$PWD/yolov3-spp-custom.cfg"
$data_file = "$PWD/thermal.data"
$name_file = "$PWD/thermal.names"
$image_dir = "$PWD/data/thermal"

if (Test-Path "$train_file") {
  Remove-Item "$train_file"
}

if (Test-Path "$valid_file") {
  Remove-Item "$valid_file"
}

For ($value = 1; $value -le 8862; $value++) {
  "data/thermal/FLIR_{0}.jpeg" -f ([string]$value).PadLeft(5, '0') | Out-File -Append -Encoding "ASCII" "$train_file"
}

For ($value = 1; $value -le 4224; $value++) {
  "data/thermal/FLIR_video_{0}.jpeg" -f ([string]$value).PadLeft(5, '0') | Out-File -Append -Encoding "ASCII" "$train_file"
}

For ($value = 8863; $value -le 10228; $value++) {
  "data/thermal/FLIR_{0}.jpeg" -f ([string]$value).PadLeft(5, '0') | Out-File -Append -Encoding "ASCII" "$train_file"
}

# Copy images to the correct directory
Remove-Item -Recurse "$image_dir"
New-Item -Path "$image_dir" -ItemType directory -Force
Copy-Item "$train_images/"* "$image_dir"
Copy-Item "$validation_images/"* "$image_dir"
Copy-Item "$video_images/"* "$image_dir"

# Convert annotations from standard COCO format to darknet format
python.exe convert_coco_yolo.py "$train_anns" "$image_dir"
python.exe convert_coco_yolo.py "$validation_anns" "$image_dir"
python.exe convert_coco_yolo.py "$video_anns" "$image_dir"

# Quick fix for imbalanced dataset
if (Test-Path "bikes.txt") {
  Remove-Item "bikes.txt"
}
For ($iter = 1; $iter -le 5; $iter++) {
  Get-Content "bikes.txt" "bikes.txt" "bikes.txt" | Set-Content "bikes_new.txt"
}
Move-Item "bikes_new.txt" "bikes.txt"
Get-Content "bikes.txt" | Add-Content "$train_file"
Remove-Item "bikes.txt"

# Shuffle train dataset
Get-Content "$train_file" | Sort-Object { Get-Random } | Out-file "train_file_shuffled.txt"
Move-Item "train_file_shuffled.txt" "$train_file"

# Copy necessary files to the correct directories
Copy-Item "$cfg_file"        "$PWD/"
Copy-Item "$train_file"      "$PWD/data/"
Copy-Item "$valid_file"      "$PWD/data/"
Copy-Item "$data_file"       "$PWD/data/"
Copy-Item "$name_file"       "$PWD/data/"
Copy-Item "run_all_iters.sh" "$PWD/"

##Download pretrained weights
#Invoke-WebRequest -Uri https://pjreddie.com/media/files/darknet53.conv.74 -OutFile "$PWD/darknet53.conv.74"

##run_all_iters.sh
#For ($iter = 17000; $iter -ge 1000; $iter -= 1000) {
#  .\darknet.exe detector map data/thermal.data yolov3-spp-custom.cfg backup/yolov3-spp-custom_${iter}.weights | Out-File -Append -Encoding "ASCII" val_res.txt
#}

##start_training.sh
#.\darknet.exe detector train data/thermal.data yolov3-spp-custom.cfg darknet53.conv.74 -dont_show -map | Out-File -Append -Encoding "ASCII" train_results.txt
