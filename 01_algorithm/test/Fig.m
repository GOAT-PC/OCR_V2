% 定义函数 f(x, y, z)
f = @(x, y, z) 6.83183653*x+2.74079513*y+144.255371*z-0.0019582675*x.^2-0.0105971189*x.*y-0.978221783*x.*z...
-0.00209583015*y.^2-0.223247168*y.*z-1.22460739*z.^2-891.4316431434478; 

% 设置 x 和 y 的网格
[x, y] = meshgrid(25:1:60, 350:2.5:530);  % x 和 y 的范围

% 创建图形窗口
figure;

% 初始化存储动图帧的数组
frames = [];

% 动态展示 z 的变化
for z_value = 0.5:0.01:2.5
    % 计算函数值
    w = f(x, y, z_value);
    
    % 绘制图形
    surf(x, y, w);
    xlabel('T');
    ylabel('V');
    zlabel('f(T, V, P)');
    title(['Surface plot for P = ', num2str(z_value)]);
    
    % 捕获当前帧
    frames = [frames; getframe(gcf)];  % 获取当前图形窗口的帧
    
    % 暂停，更新图像
    pause(0.1);
end

% 创建动图并保存
filename = 'dynamic_surface.gif';  % 设置动图保存文件名
for i = 1:length(frames)
    % 将帧保存为GIF文件
    [imind, cm] = rgb2ind(frame2im(frames(i)), 256);
    if i == 1
        imwrite(imind, cm, filename, 'gif', 'LoopCount', inf, 'DelayTime', 0.5);
    else
        imwrite(imind, cm, filename, 'gif', 'WriteMode', 'append', 'DelayTime', 0.1);
    end
end

disp('动态GIF已保存！');
