function [dArray,eArray,tArray,RSD_eArray,RSD_dArray,deltaRSDArray,stepArray] = runOptTest(fileName,tSecs,talk,motor_pos,tExpt)
close all



%% settings
maxSteps = 80;
RSDthreshold = 0.01;
RSDmax = 0.1;
tPause = 0;
nSteps = ceil((tExpt * 1.05 * 60) / tSecs);
nSamples = 250;
deltaRSD_Old = -1;

%% define storage arrays for plotting
stepArray = zeros(nSteps,1);
tArray = zeros(nSteps,nSamples);
eArray = zeros(nSteps,nSamples);
dArray = zeros(nSteps,nSamples);

%% Optimisation Routine
for j = 1:nSteps
    
    
    %% store motor position
    %disp(['Step Number: ' num2str(j) ', Motor Position: ' num2str(motor_pos)])
    stepArray(j) = motor_pos;
    
    %% initialise counters
    sumX_d = 0;
    sumX2_d = 0;
    sumX_e = 0;
    sumX2_e = 0;
    
    %% acquire data and update counters
    tic
    i = 0;
    while toc <= tSecs
        i = i + 1;
        fprintf(talk,'D');
        dNow = str2double(fscanf(talk));
        fprintf(talk,'E');
        eNow = str2double(fscanf(talk));
        sumX_d = sumX_d + dNow;
        sumX2_d = sumX2_d + dNow*dNow;
        sumX_e = sumX_e + eNow;
        sumX2_e = sumX2_e + eNow*eNow;
        
        dArray(j,i) = dNow;
        eArray(j,i) = eNow;
        tArray(j,i) = toc;
    end
    
    %% calculate parameters
    mu_d = sumX_d/i;
    sigma_d = sqrt(sumX2_d/i - mu_d*mu_d);
    mu_e = sumX_e/i;
    sigma_e = sqrt(sumX2_e/i - mu_e*mu_e);
    tNow = (tSecs+tPause)*j;
    RSD_d = sigma_d/mu_d;
    RSD_e = sigma_e/mu_e;
    deltaRSD = RSD_e-RSD_d;
    
    %% update and store arrays for plotting
    
    RSD_dArray(j) = RSD_d;
    RSD_eArray(j) = RSD_e;
    deltaRSDArray(j) = deltaRSD;
    assignin('base','tArray', tArray);
    assignin('base','dArray', dArray);
    assignin('base','eArray', eArray);
    assignin('base','stepArray', stepArray);
    assignin('base','RSD_eArray', RSD_eArray);
    assignin('base','RSD_dArray', RSD_dArray);
    assignin('base','deltaRSDArray', deltaRSDArray);
    assignin('base','sigma_e', sigma_e);
    assignin('base','sigma_d', sigma_d);
    assignin('base','motor_pos', motor_pos);
    
    %% plot data
    f = figure(1);
    subplot(2,2,1);
    plot(tArray(j,1:i),eArray(j,1:i),'r-',tArray(j,1:i),dArray(j,1:i),'-b'); legend('End','Wall');
    title('Traces');
    subplot(2,2,2);
    plot(1:j,RSD_eArray(1:j),'-or',1:j,RSD_dArray(1:j),'-ob'); legend('End','Wall');
    title('Relative Standard Deviations');
    subplot(2,2,3);
    plot(1:j,stepArray(1:j),'-ob'); legend('End-Wall');
    title('Motor vs Iteration');
    subplot(2,2,4);
    plot(1:j,deltaRSDArray,'-ok'); legend('Delta');
    title('Net Relative Standard Deviation');
    [ax,h3]=suplabel(['Step: ' num2str(j) ', Time: ' num2str(j*(tSecs+tPause)), ', Motor: ' num2str(motor_pos)] ,'t');
    set(h3,'FontSize',18);
    drawnow
    
    %     savefig(['Plot ' num2str(j)]);
    
    %% update step size
    stepSize = getStepSize(deltaRSD,RSDthreshold,RSDmax,maxSteps);
    if deltaRSD*deltaRSD_Old < 0
        stepSize = min(stepSize,20);
    end
    
    %% move motor
    motor_pos = moveMotor(deltaRSD,stepSize,stepSize,motor_pos,talk);
    
    %% wait to stabilise
    pause(tPause);
    
    deltaRSD_Old = deltaRSD;
    
end

fwrite(talk,['C' num2str(motor_pos) '\n']);
save(fileName, 'dArray','eArray','tArray','deltaRSDArray','RSD_dArray','RSD_eArray','stepArray');

end

function motor_pos = moveMotor(deltaRSD,openSteps,closeSteps,motor_pos,talk)
    if deltaRSD < 0
        fprintf(talk,['A' num2str(openSteps) '\n']);
        motor_pos = motor_pos + openSteps;
    elseif deltaRSD > 0
        fprintf(talk,['C' num2str(closeSteps) '\n']);
        motor_pos = motor_pos - closeSteps;
    end
end
