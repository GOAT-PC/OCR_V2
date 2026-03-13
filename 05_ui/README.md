# OCR UI README

## 1. 目录定位

`05_ui` 是当前项目里独立的前端 UI 原型目录，使用 `Vite + React + Tailwind CSS` 搭建。

这个目录当前的职责是：

- 展示一套 OCR 仪器面板风格的前端界面
- 模拟设备开关、电源、急停、数据展示和 AI 诊断弹窗
- 作为后续对接串口数据、算法输出、上位机数据流的 UI 容器

当前它还不是完整的上位机程序，也没有接入真实硬件数据。

## 2. 目录结构

```text
05_ui/
├─ index.html
├─ package.json
├─ package-lock.json
├─ postcss.config.js
├─ tailwind.config.js
├─ vite.config.mjs
├─ src/
│  ├─ main.jsx
│  ├─ OcrPanel.jsx
│  └─ index.css
└─ node_modules/               # 依赖安装结果，不属于手写源码
```

## 3. 关键文件说明

### `index.html`

- Vite 的页面入口
- 提供 `#root` 挂载点

### `src/main.jsx`

- React 入口文件
- 将 `OcrPanel` 挂载到页面

### `src/OcrPanel.jsx`

- 当前 UI 的核心文件
- 包含整套面板布局和交互逻辑
- 主要状态有：
  - `powerOn`：电源开关状态
  - `eStopTriggered`：急停状态
  - `showAiModal`：AI 诊断弹窗开关
  - `isAnalyzing`：AI 请求中的加载状态
  - `aiAnalysis`：AI 返回文本

### `src/index.css`

- Tailwind 基础样式入口
- 补充全局背景、文本和根节点布局样式

### `tailwind.config.js`

- Tailwind 扫描路径和主题扩展
- 当前扩展了少量颜色、阴影和字体配置

### `postcss.config.js`

- PostCSS 配置
- 启用 `tailwindcss` 和 `autoprefixer`

### `vite.config.mjs`

- Vite 配置
- 当前开发端口固定为 `5173`

## 4. 当前 UI 功能

当前界面已经实现的内容：

- 仪器外壳和屏幕风格布局
- 顶部设备信息显示
- OCR、压力、温度的静态展示
- 模拟趋势图
- 急停按钮交互
- 电源开关交互
- AI 诊断弹窗
- 通过 `fetch` 调用 Gemini 接口并显示返回结果

当前界面里展示的数据仍然是写死的示例值：

- OCR：`12.45 %`
- Pressure：`2.14 MPa`
- Temperature：`65.2 °C`
- Refrigerant：`R134a`
- Oil：`POE 68`

## 5. 运行方式

在 `05_ui` 目录下执行：

```bash
npm install
npm run dev
```

默认访问：

```text
http://localhost:5173
```

构建生产包：

```bash
npm run build
```

预览构建结果：

```bash
npm run preview
```

## 6. 复现这个 UI 文件夹时，最少需要保留的文件

如果要从零复现当前 UI，至少需要以下手写文件：

```text
05_ui/
├─ index.html
├─ package.json
├─ postcss.config.js
├─ tailwind.config.js
├─ vite.config.mjs
└─ src/
   ├─ main.jsx
   ├─ OcrPanel.jsx
   └─ index.css
```

其中：

- `package-lock.json` 可以重新生成
- `node_modules/` 不需要纳入源码管理

## 7. 当前限制

- 还没有真实串口、USB、LIN 或 WebSocket 数据接入
- 还没有统一的数据模型
- `OcrPanel.jsx` 同时承担了布局、状态和外部请求逻辑，后续建议拆出数据适配层
- AI 调用里 `apiKey` 目前是空字符串，默认依赖外部运行环境注入
- 导出报表、开始测量、工质配置等按钮目前只有 UI，没有真实业务逻辑

## 8. 后续对接建议

后续推荐保持这个 UI 的视觉结构不动，只增加接口层：

- 新增 `data adapter`：把设备/算法输出转换成 UI 需要的数据结构
- 新增 `service`：封装 AI 诊断、设备通信、数据缓存
- 新增统一状态对象：如 `ocr_concentration`、`pressure`、`temperature`、`device_status`
- 让 `OcrPanel.jsx` 只消费格式化后的数据，不直接承担协议解析

## 9. 备注

这个目录目前更接近“前端展示原型”或“上位机 UI 雏形”，不是完整的设备联调版本。

如果后续要进入联调阶段，建议优先在不破坏 `OcrPanel.jsx` 现有布局和交互风格的前提下，加一层数据输入适配。
