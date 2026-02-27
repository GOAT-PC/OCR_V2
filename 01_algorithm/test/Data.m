load OCR_Data.data
[m, n] = size(OCR_Data);
rand_indices = randperm(m, 186);
remaining_indices = setdiff(1:m, rand_indices);
OCR_TrainData = OCR_Data(remaining_indices, :);
OCR_VerifyData = OCR_Data(rand_indices, :);

writematrix(OCR_TrainData, 'OCR_TrainData.txt', 'Delimiter', 'tab');  % 使用 tab 作为分隔符
writematrix(OCR_VerifyData, 'OCR_VerifyData.txt', 'Delimiter', 'tab');