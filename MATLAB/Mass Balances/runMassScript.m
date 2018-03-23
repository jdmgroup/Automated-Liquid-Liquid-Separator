balance = setupSatoriusBalance('COM61');
bigbalance = setupSatoriusBigBalance('COM62');

tareSatoriusBalance(balance);
tareSatoriusBigBalance(bigbalance);

filePath = 'C:\Users\jdmgroup\Documents\MATLAB\Martin\Actual\Mass Balance\Test8\';
fileName = 'JHB_13';
fileName = [filePath fileName];

tic;
i = 1;
%% loop
while 1
    m_toluene = getMassfromSatoriusBigBalance(bigbalance);
    m_water = getMassfromSatoriusBalance(balance);
    tol_Array(i) = m_toluene;
    water_Array(i) = m_water;
    t_tol_Array(i) = toc;
    t_water_Array(i) = toc;
    
    assignin('base','tol_Array', tol_Array);
    assignin('base','water_Array', water_Array);
    assignin('base','t_tol_Array', t_tol_Array);
    assignin('base','t_water_Array', t_water_Array);
    save(fileName, 't_tol_Array','tol_Array','t_water_Array','water_Array');
    
    figure(2);
    plot(t_water_Array, water_Array, 'b', t_tol_Array, tol_Array,'r','linewidth',2)
    l = legend('water', 'toluene');
    set(l,'box','off');
    xlabel('Time / s','FontName','Arial','FontSize',16)
    ylabel('Accumulated Mass / g','FontName','Arial','FontSize',16)
    set(gca, 'FontName','Arial','FontSize',14)
    drawnow
    i = i + 1;
    %pause(1);
end

savefig('my mass data')
closeSatoriusBalance(balance)
closeSatoriusBigBalance(bigbalance)