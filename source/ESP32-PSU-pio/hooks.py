import os, glob, shutil

Import("env")

for f in glob.glob(os.path.join(env["PROJECT_DIR"], os.path.pardir, 'Control_C_ADS_Touch_3_5', '*.cpp')):
    shutil.copyfile(f, os.path.join(env["PROJECT_SRC_DIR"],os.path.basename(f)))
    
for f in glob.glob(os.path.join(env["PROJECT_DIR"], os.path.pardir, 'Control_C_ADS_Touch_3_5', '*.h')):
    shutil.copyfile(f, os.path.join(env["PROJECT_SRC_DIR"],os.path.basename(f)))
