function motor_pos = moveMotor(deltaRSD,openSteps,closeSteps,motor_pos,talk)
if deltaRSD < - threshold
    fprintf(talk,['A' num2str(openSteps) '\n']);
    motor_pos = motor_pos + openSteps;
elseif deltaRSD > threshold
    fprintf(talk,['C' num2str(closeSteps) '\n']);
    motor_pos = motor_pos - closeSteps;
end
end