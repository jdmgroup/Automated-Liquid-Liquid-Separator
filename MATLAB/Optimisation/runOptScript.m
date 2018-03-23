if (exist('talk','var'))
    fclose(talk);
end

clear all
close all

expt_time = 10; % mins

if ispc
    filePath = ['C:\Users\jdmgroup\Documents\MATLAB\James\'];
end

if ismac
    filePath = ['/Users/' getenv('USER') '/Documents/MATLAB/AutoSeparator/'];
end

returned_data = inputdlg('Enter Expt Name','Expt Name');
fileName = returned_data{1};
if exist(fullfile(filePath,fileName),'dir') == 0
    mkdir(filePath,fileName);
    display('Experiment folder created!')
else
    error('Experiment name already exists! Try again with another name')
end
fileName = fullfile(filePath, fileName, fileName);

%% setup port
talk = serial('COM16','BaudRate',115200);
fopen(talk);
pause(120);
motor_pos = 0;

[dArray,eArray,tArray,RSD_eArray,RSD_dArray,deltaRSDArray,stepArray] = runOptTest(fileName,5,talk,motor_pos,expt_time);

%% close port
fclose(talk);
clear talk