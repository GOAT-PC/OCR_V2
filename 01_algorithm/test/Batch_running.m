model_name = 'OCRtest';
t_stop = 1;
set_param('OCRtest', 'StopTime', 't_stop');

load OCR_Data.data

T = OCR_Data(:,1);
V = OCR_Data(:,2);
P = OCR_Data(:,3);
W = OCR_Data(:,4);

[num_simulations,~] = size(T);

output = zeros(num_simulations,4);

for i = 1:num_simulations
    input_T = T(i);
    input_V = V(i);
    input_P = P(i);
    sim(model_name);
    output(i,1) = W(i);
    output(i,2) = ans.W;
    output(i,3) = W(i)-ans.W;
    output(i,4) = (W(i)-ans.W)/W(i)*100;
end

error_meam = mean(output(:,4));
disp(['误差平均值为：' num2str(error_meam) '%']);