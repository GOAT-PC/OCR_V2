import React, { useState } from 'react';
import { Usb, Power, AlertTriangle, Activity, Settings, Database, Play, Sparkles, X } from 'lucide-react';

export default function OcrPanel() {
  const [powerOn, setPowerOn] = useState(true);
  const [eStopTriggered, setEStopTriggered] = useState(false);
  
  // AI 诊断相关状态
  const [showAiModal, setShowAiModal] = useState(false);
  const [isAnalyzing, setIsAnalyzing] = useState(false);
  const [aiAnalysis, setAiAnalysis] = useState("");

  // 模拟急停
  const triggerEStop = () => {
    setEStopTriggered(!eStopTriggered);
    if (!eStopTriggered) {
      setPowerOn(false);
      setShowAiModal(false);
    }
  };

  // 带指数退避的 API 请求封装
  const fetchWithRetry = async (url, options, retries = 5) => {
    const delays = [1000, 2000, 4000, 8000, 16000];
    for (let i = 0; i < retries; i++) {
      try {
        const response = await fetch(url, options);
        // 使用模板字符串构造更易读的错误信息
        if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
        return await response.json();
      } catch (error) {
        if (i === retries - 1) throw error;
        await new Promise(res => setTimeout(res, delays[i]));
      }
    }
  };

  // 触发 Gemini AI 大模型分析
  const handleAiDiagnosis = async () => {
    setShowAiModal(true);
    setIsAnalyzing(true);
    setAiAnalysis("");

    // 收集当前界面上的模拟工业数据作为 Prompt 上下文
    const prompt = `你现在是一位资深的工业制冷系统与热力学专家。
当前我们的一台 OCR 在线含油率分析仪测得以下实时台架数据：
- 系统运行冷媒：R134a
- 冷冻油类型：POE 68
- 实时含油率 (OCR)：12.45%
- 实时系统压力：2.14 MPa
- 实时流体温度：65.2 °C

请根据以上物理参数，用极其精炼、专业的中文给出：
1. 【工况评估】：评估当前的温压匹配关系以及 12.45% 的含油率在常规系统中是否偏高或偏低。
2. 【异常风险】：指出可能存在的系统隐患（如排气温度过高、压缩机磨损、或回油不畅等）。
3. 【维护建议】：给出一条给现场操作员的直接建议。`;

    try {
      const apiKey = ""; // 运行环境将自动注入 Key
      const url = `https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash-preview-09-2025:generateContent?key=${apiKey}`;
      
      const payload = {
        contents: [{ parts: [{ text: prompt }] }],
        systemInstruction: { 
          parts: [{ text: "你是一个专业的工业仪表AI诊断助手，回答务必简短、直接、充满工程严谨性，切忌废话。" }] 
        }
      };

      const data = await fetchWithRetry(url, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });

      const text = data.candidates?.[0]?.content?.parts?.[0]?.text;
      setAiAnalysis(text || "AI未能生成有效分析，请重试。");
    } catch (error) {
      setAiAnalysis("⚠️ 连接到 AI 诊断核心失败。请检查网络配置或重试。");
    } finally {
      setIsAnalyzing(false);
    }
  };

  return (
    <div className="min-h-screen bg-gray-200 flex items-center justify-center p-8 font-sans">
      {/* 仪器外壳 (Chassis) */}
      <div className="relative w-full max-w-5xl bg-gradient-to-br from-slate-700 to-slate-900 p-3 rounded-2xl shadow-2xl border-b-8 border-r-8 border-slate-950">
        
        {/* 前面板金属拉丝质感 */}
        <div className="w-full bg-slate-800 rounded-xl p-6 flex flex-row gap-6 border border-slate-600 shadow-inner h-[500px]">
          
          {/* ================= 1. 电容触摸屏区域 ================= */}
          <div className="flex-grow bg-black rounded-lg p-2 shadow-[inset_0_0_20px_rgba(0,0,0,1)] border-4 border-slate-900 relative">
            {powerOn ? (
              <div className="w-full h-full bg-slate-900 rounded flex flex-col overflow-hidden relative">
                
                {/* 屏幕顶栏 */}
                <div className="h-12 bg-slate-800 border-b border-slate-700 flex items-center justify-between px-4 text-slate-300">
                  <div className="flex items-center gap-2">
                    <Activity size={20} className="text-cyan-400" />
                    <span className="font-bold tracking-widest">OCR ANALYZER PRO</span>
                  </div>
                  <div className="flex items-center gap-4 text-sm">
                    <span className="px-2 py-1 bg-slate-700 rounded text-cyan-300">R134a</span>
                    <span className="px-2 py-1 bg-slate-700 rounded text-cyan-300">POE 68</span>
                    <span>14:32:05</span>
                  </div>
                </div>

                {/* 核心数据区 */}
                <div className="flex-grow p-6 flex flex-col relative">
                  <div className="flex justify-between items-start mb-6">
                    <div>
                      <h2 className="text-slate-400 text-lg mb-1">实时含油率 (OCR)</h2>
                      <div className="text-6xl font-bold text-cyan-400 font-mono tracking-tighter">
                        12.45 <span className="text-3xl text-slate-500">%</span>
                      </div>
                    </div>
                    <div className="flex flex-col gap-3 text-right">
                      <div className="bg-slate-800/50 p-3 rounded-lg border border-slate-700">
                        <div className="text-slate-400 text-xs uppercase mb-1">系统压力</div>
                        <div className="text-xl font-mono text-white">2.14 <span className="text-sm text-slate-500">MPa</span></div>
                      </div>
                      <div className="bg-slate-800/50 p-3 rounded-lg border border-slate-700">
                        <div className="text-slate-400 text-xs uppercase mb-1">流体温度</div>
                        <div className="text-xl font-mono text-white">65.2 <span className="text-sm text-slate-500">°C</span></div>
                      </div>
                    </div>
                  </div>

                  {/* 模拟趋势图 */}
                  <div className="flex-grow bg-slate-950 rounded-lg border border-slate-800 relative overflow-hidden flex items-end p-0">
                    <div className="absolute top-3 left-4 text-xs text-slate-500">实时浓度趋势 (10Hz)</div>
                    <svg className="w-full h-32" preserveAspectRatio="none" viewBox="0 0 100 100">
                      <path d="M0,80 Q10,75 20,80 T40,60 T60,50 T80,45 T100,48 L100,100 L0,100 Z" fill="rgba(34, 211, 238, 0.1)" />
                      <path d="M0,80 Q10,75 20,80 T40,60 T60,50 T80,45 T100,48" fill="none" stroke="#22d3ee" strokeWidth="2" />
                    </svg>
                  </div>
                  
                  {/* ====== AI 智能诊断遮罩层 ====== */}
                  {showAiModal && (
                    <div className="absolute inset-0 bg-slate-900/95 backdrop-blur-sm z-20 flex flex-col overflow-hidden">
                      <div className="flex justify-between items-center p-4 border-b border-slate-700 bg-slate-800">
                        <div className="flex items-center gap-2 text-purple-400 font-bold">
                          <Sparkles size={18} />
                          <span>Gemini 智能工况诊断</span>
                        </div>
                        <button 
                          onClick={() => setShowAiModal(false)}
                          className="text-slate-400 hover:text-white transition"
                        >
                          <X size={20} />
                        </button>
                      </div>
                      <div className="flex-grow p-5 overflow-y-auto text-slate-200">
                        {isAnalyzing ? (
                          <div className="flex flex-col items-center justify-center h-full gap-4">
                            <div className="w-10 h-10 border-4 border-purple-500 border-t-transparent rounded-full animate-spin"></div>
                            <p className="text-slate-400 text-sm animate-pulse">正在提取底层热力学特征，请求云端 AI 诊断...</p>
                          </div>
                        ) : (
                          <div className="whitespace-pre-wrap text-sm leading-relaxed font-mono">
                            {aiAnalysis}
                          </div>
                        )}
                      </div>
                    </div>
                  )}
                </div>

                {/* 底部功能栏 (UI内) */}
                <div className="h-16 bg-slate-800/80 border-t border-slate-700 flex items-center justify-between px-6 z-10">
                  <div className="flex gap-3">
                    <button className="flex items-center gap-2 bg-slate-700 hover:bg-slate-600 px-3 py-2 rounded text-sm text-white transition">
                      <Settings size={16} /> 工质配置
                    </button>
                    <button className="flex items-center gap-2 bg-slate-700 hover:bg-slate-600 px-3 py-2 rounded text-sm text-white transition">
                      <Database size={16} /> 导出报表
                    </button>
                    {/* 唤起 AI 诊断的按钮 */}
                    <button 
                      onClick={handleAiDiagnosis}
                      disabled={isAnalyzing || showAiModal}
                      className="flex items-center gap-1 bg-purple-600/20 border border-purple-500/50 hover:bg-purple-600/40 text-purple-300 px-3 py-2 rounded text-sm transition shadow-[0_0_10px_rgba(168,85,247,0.2)]"
                    >
                      <Sparkles size={16} className="text-purple-400" /> 
                      <span className="font-bold text-purple-200">✨ 智能诊断</span>
                    </button>
                  </div>
                  <button className="flex items-center gap-2 bg-cyan-600 hover:bg-cyan-500 px-6 py-2 rounded font-bold text-white transition shadow-[0_0_15px_rgba(8,145,178,0.5)]">
                    <Play size={16} /> 开始测录
                  </button>
                </div>
              </div>
            ) : (
              <div className="w-full h-full bg-black flex items-center justify-center">
                {eStopTriggered && (
                  <div className="text-red-500 flex flex-col items-center animate-pulse">
                    <AlertTriangle size={64} className="mb-4" />
                    <span className="text-xl font-bold tracking-widest">SYSTEM HALTED</span>
                  </div>
                )}
              </div>
            )}
            
            {/* 屏幕玻璃反光效果 */}
            <div className="absolute inset-0 bg-gradient-to-tr from-white/0 via-white/5 to-white/0 pointer-events-none rounded-lg z-30"></div>
          </div>

          {/* ================= 2. 右侧物理控制区 ================= */}
          <div className="w-40 flex flex-col justify-between py-2 relative">
            
            {/* 品牌标识位 */}
            <div className="text-center pb-4 border-b border-slate-700">
              <span className="text-slate-400 font-bold tracking-widest text-sm">INSTRUMENTS</span>
            </div>

            {/* A. 物理急停按钮 (E-STOP) */}
            <div className="flex flex-col items-center mt-6">
              <div className="text-xs text-slate-400 font-bold mb-2 tracking-wider">EMERGENCY STOP</div>
              <button 
                onClick={triggerEStop}
                className={`w-24 h-24 rounded-full flex items-center justify-center shadow-2xl transition-all duration-200 border-4 border-yellow-500 relative
                  ${eStopTriggered 
                    ? 'bg-red-700 scale-95 shadow-[inset_0_10px_20px_rgba(0,0,0,0.6)]' 
                    : 'bg-red-500 hover:bg-red-400 shadow-[0_10px_20px_rgba(0,0,0,0.5),inset_0_-5px_15px_rgba(0,0,0,0.3)]'
                  }`}
              >
                <div className="w-16 h-16 rounded-full border-2 border-red-400 opacity-50 absolute"></div>
                <span className="text-white font-black text-sm drop-shadow-md">PUSH</span>
              </button>
            </div>

            <div className="flex-grow"></div>

            {/* B. USB 接口 */}
            <div className="flex flex-col items-center mb-10">
              <div className="text-xs text-slate-400 mb-2 flex items-center gap-1"><Usb size={14}/> DATA EXPORT</div>
              <div className="w-16 h-8 bg-black rounded border-2 border-slate-500 flex items-center justify-center shadow-inner relative group cursor-pointer hover:border-slate-400 transition">
                <div className="w-12 h-3 bg-zinc-800 border border-zinc-900 flex justify-center items-center">
                  <div className="w-8 h-1 bg-black"></div>
                </div>
                {/* 插入U盘提示 */}
                <div className="absolute top-10 w-24 text-center text-[10px] text-cyan-400 opacity-0 group-hover:opacity-100 transition-opacity">
                  Insert USB Drive
                </div>
              </div>
            </div>

            {/* C. 物理电源开关 */}
            <div className="flex flex-col items-center">
              <div className="text-xs text-slate-400 mb-2 font-bold tracking-wider">POWER</div>
              <button 
                onClick={() => { if(!eStopTriggered) setPowerOn(!powerOn) }}
                className={`w-14 h-20 rounded-lg flex flex-col justify-center items-center border border-slate-900 transition-all duration-300 shadow-xl
                  ${powerOn 
                    ? 'bg-slate-700 shadow-[inset_0_-5px_10px_rgba(0,0,0,0.5)]' 
                    : 'bg-slate-800 shadow-[inset_0_5px_10px_rgba(0,0,0,0.8)]'
                  }`}
              >
                <div className={`w-10 h-8 rounded mb-1 border-b-2 flex justify-center items-center
                  ${powerOn ? 'bg-green-500 border-green-700 shadow-[0_0_10px_rgba(34,197,94,0.5)]' : 'bg-slate-900 border-black'}`}>
                  <span className={`text-[10px] font-bold ${powerOn ? 'text-white' : 'text-slate-600'}`}>I</span>
                </div>
                <div className={`w-10 h-8 rounded mt-1 border-t-2 flex justify-center items-center
                  ${!powerOn ? 'bg-slate-700 border-slate-600' : 'bg-slate-900 border-black'}`}>
                  <span className={`text-[10px] font-bold ${!powerOn ? 'text-white' : 'text-slate-600'}`}>O</span>
                </div>
              </button>
            </div>

          </div>
        </div>
      </div>
    </div>
  );
}


