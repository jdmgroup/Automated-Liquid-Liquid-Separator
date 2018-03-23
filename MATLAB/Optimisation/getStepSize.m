function newStep = getStepSize(RSD_In,RSDthreshold,RSDmax,stepMax)

RSD_In = abs(RSD_In); %calculates step size based on RSD size only
nDiv = stepMax+1; %number of discrete step sizes
rMin = RSDthreshold;
rMax = RSDmax;

%Parameters to create array of step sizes
stepMin = 0;
dStep = (stepMax-stepMin)/(nDiv-1);

%Parameters to create array of RSD values
alpha = 0.0;
alpha = max(0.0001,alpha); %avoids numeric errors at zero
rMin = log10(rMin);
rMax = log10(rMax);
dx = (rMax-rMin)/(nDiv-1);

%create arrays
for i = 1:nDiv
    step(i) = stepMin + (i-1)*dStep;
    RSD(i) = 10^(rMin + (i-1)*dx);
    RSD(i) = RSD(i)^alpha;
end

%rescale RSD
scaleFactor = (10^rMax-10^rMin)/(RSD(nDiv)-RSD(1));
offSet = 10^rMin-scaleFactor*RSD(1);
for i = 1:nDiv
    RSD(i) = offSet + RSD(i)*scaleFactor;
end

%find step size corresponding to nearest RSD value
dBest = 1000;
for i = 1:nDiv
    d = abs(RSD_In-RSD(i));
    if d < dBest
        dBest = d;
        newStep = step(i);
    end       
end

% figure(1);
% plot(RSD,step,'-');
% xlabel('RSD value');
% ylabel('Step size');

end


