#ifndef DASHBOARD_H
#define DASHBOARD_H

const char DASHBOARD_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>EchoNode // Pentest Suite</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600;700&family=Inter:wght@400;500;600;700&display=swap');
        :root {
            --bg-primary: #0a0e17;
            --bg-secondary: #111827;
            --bg-card: #1a1f2e;
            --bg-card-hover: #222838;
            --neon: #00ff88;
            --neon-dim: #00cc6a;
            --neon-glow: rgba(0, 255, 136, 0.15);
            --neon-glow-strong: rgba(0, 255, 136, 0.3);
            --cyan: #00e5ff;
            --red: #ff3355;
            --red-dim: #cc2944;
            --orange: #ff9500;
            --text: #e2e8f0;
            --text-muted: #64748b;
            --border: rgba(255,255,255,0.06);
            --font-mono: 'JetBrains Mono', 'Courier New', monospace;
            --font-sans: 'Inter', -apple-system, sans-serif;
        }
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: var(--font-sans);
            background: var(--bg-primary);
            color: var(--text);
            min-height: 100vh;
            overflow-x: hidden;
        }

        /* HEADER */
        .header {
            background: var(--bg-secondary);
            border-bottom: 1px solid var(--neon);
            box-shadow: 0 2px 20px var(--neon-glow);
            padding: 0.75rem 1.5rem;
            display: flex;
            align-items: center;
            justify-content: space-between;
            position: sticky;
            top: 0;
            z-index: 100;
        }
        .logo {
            font-family: var(--font-mono);
            font-weight: 700;
            font-size: 1.1rem;
            color: var(--neon);
            text-shadow: 0 0 10px var(--neon-glow-strong);
            letter-spacing: 2px;
        }
        .logo span { color: var(--text-muted); font-weight: 400; }
        .status-badge {
            font-family: var(--font-mono);
            font-size: 0.7rem;
            padding: 4px 10px;
            border-radius: 20px;
            background: rgba(0,255,136,0.1);
            border: 1px solid var(--neon-dim);
            color: var(--neon);
            animation: pulse-border 2s infinite;
        }
        @keyframes pulse-border {
            0%, 100% { box-shadow: 0 0 5px var(--neon-glow); }
            50% { box-shadow: 0 0 15px var(--neon-glow-strong); }
        }

        /* TAB NAV */
        .tab-nav {
            display: flex;
            background: var(--bg-secondary);
            border-bottom: 1px solid var(--border);
            overflow-x: auto;
            -webkit-overflow-scrolling: touch;
        }
        .tab-btn {
            font-family: var(--font-mono);
            font-size: 0.8rem;
            padding: 0.85rem 1.25rem;
            background: none;
            border: none;
            color: var(--text-muted);
            cursor: pointer;
            transition: all 0.25s;
            white-space: nowrap;
            position: relative;
            letter-spacing: 0.5px;
        }
        .tab-btn:hover { color: var(--neon); background: rgba(0,255,136,0.05); }
        .tab-btn.active {
            color: var(--neon);
            text-shadow: 0 0 8px var(--neon-glow);
        }
        .tab-btn.active::after {
            content: '';
            position: absolute;
            bottom: 0; left: 0; right: 0;
            height: 2px;
            background: var(--neon);
            box-shadow: 0 0 10px var(--neon);
        }
        .tab-icon { margin-right: 6px; }

        /* MAIN CONTENT */
        .content { padding: 1.25rem; max-width: 800px; margin: 0 auto; }
        .tab-panel { display: none; animation: fadeIn 0.3s; }
        .tab-panel.active { display: block; }
        @keyframes fadeIn { from { opacity: 0; transform: translateY(8px); } to { opacity: 1; transform: translateY(0); } }

        /* CARDS */
        .card {
            background: var(--bg-card);
            border: 1px solid var(--border);
            border-radius: 12px;
            padding: 1.25rem;
            margin-bottom: 1rem;
            transition: all 0.25s;
        }
        .card:hover { border-color: rgba(0,255,136,0.2); background: var(--bg-card-hover); }
        .card-title {
            font-family: var(--font-mono);
            font-size: 0.75rem;
            color: var(--neon);
            text-transform: uppercase;
            letter-spacing: 2px;
            margin-bottom: 1rem;
            display: flex;
            align-items: center;
            gap: 8px;
        }
        .card-title::before {
            content: '>';
            color: var(--neon);
            font-weight: bold;
        }

        /* METRIC GRID */
        .metric-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 0.75rem;
        }
        .metric {
            background: rgba(0,0,0,0.3);
            border: 1px solid var(--border);
            border-left: 3px solid var(--neon);
            border-radius: 8px;
            padding: 0.85rem;
        }
        .metric-label {
            font-size: 0.7rem;
            color: var(--text-muted);
            text-transform: uppercase;
            letter-spacing: 1px;
            margin-bottom: 0.35rem;
        }
        .metric-value {
            font-family: var(--font-mono);
            font-size: 1.1rem;
            font-weight: 600;
            color: var(--neon);
        }

        /* BUTTONS */
        .btn {
            font-family: var(--font-mono);
            font-size: 0.8rem;
            padding: 0.65rem 1.25rem;
            border: 1px solid;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.25s;
            display: inline-flex;
            align-items: center;
            gap: 6px;
            letter-spacing: 0.5px;
        }
        .btn-primary {
            background: rgba(0,255,136,0.1);
            border-color: var(--neon);
            color: var(--neon);
        }
        .btn-primary:hover {
            background: rgba(0,255,136,0.2);
            box-shadow: 0 0 20px var(--neon-glow);
        }
        .btn-danger {
            background: rgba(255,51,85,0.1);
            border-color: var(--red);
            color: var(--red);
        }
        .btn-danger:hover {
            background: rgba(255,51,85,0.2);
            box-shadow: 0 0 20px rgba(255,51,85,0.3);
        }
        .btn-warning {
            background: rgba(255,149,0,0.1);
            border-color: var(--orange);
            color: var(--orange);
        }
        .btn-warning:hover {
            background: rgba(255,149,0,0.2);
            box-shadow: 0 0 20px rgba(255,149,0,0.3);
        }
        .btn:disabled {
            opacity: 0.4;
            cursor: not-allowed;
        }
        .btn-group { display: flex; gap: 0.5rem; flex-wrap: wrap; }

        /* SCANNER TABLE */
        .scan-table {
            width: 100%;
            border-collapse: collapse;
            font-family: var(--font-mono);
            font-size: 0.75rem;
            margin-top: 0.75rem;
        }
        .scan-table th {
            text-align: left;
            padding: 0.6rem 0.5rem;
            border-bottom: 1px solid var(--neon-dim);
            color: var(--neon);
            font-size: 0.65rem;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .scan-table td {
            padding: 0.55rem 0.5rem;
            border-bottom: 1px solid var(--border);
            color: var(--text);
            max-width: 120px;
            overflow: hidden;
            text-overflow: ellipsis;
            white-space: nowrap;
        }
        .scan-table tr {
            cursor: pointer;
            transition: background 0.2s;
        }
        .scan-table tr:hover { background: rgba(0,255,136,0.05); }
        .scan-table tr.selected { background: rgba(0,255,136,0.1); border-left: 2px solid var(--neon); }

        /* SIGNAL BARS */
        .signal { display: flex; align-items: flex-end; gap: 2px; height: 14px; }
        .signal-bar {
            width: 3px;
            border-radius: 1px;
            background: var(--text-muted);
            transition: background 0.3s;
        }
        .signal-bar.active { background: var(--neon); }
        .signal-bar:nth-child(1) { height: 4px; }
        .signal-bar:nth-child(2) { height: 7px; }
        .signal-bar:nth-child(3) { height: 10px; }
        .signal-bar:nth-child(4) { height: 14px; }

        /* ENCRYPTION BADGE */
        .enc-badge {
            font-size: 0.6rem;
            padding: 2px 6px;
            border-radius: 4px;
            font-weight: 600;
        }
        .enc-open { background: rgba(255,51,85,0.15); color: var(--red); border: 1px solid rgba(255,51,85,0.3); }
        .enc-wep { background: rgba(255,149,0,0.15); color: var(--orange); border: 1px solid rgba(255,149,0,0.3); }
        .enc-wpa { background: rgba(0,229,255,0.15); color: var(--cyan); border: 1px solid rgba(0,229,255,0.3); }
        .enc-wpa2 { background: rgba(0,255,136,0.15); color: var(--neon); border: 1px solid rgba(0,255,136,0.3); }

        /* ATTACK PANEL */
        .attack-section {
            border: 1px solid var(--border);
            border-radius: 10px;
            padding: 1rem;
            margin-bottom: 1rem;
            background: rgba(0,0,0,0.2);
        }
        .attack-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 0.75rem;
        }
        .attack-title {
            font-family: var(--font-mono);
            font-size: 0.85rem;
            font-weight: 600;
        }
        .attack-desc {
            font-size: 0.75rem;
            color: var(--text-muted);
            margin-bottom: 0.85rem;
            line-height: 1.5;
        }
        .target-info {
            font-family: var(--font-mono);
            font-size: 0.7rem;
            padding: 0.5rem 0.75rem;
            background: rgba(0,0,0,0.4);
            border-radius: 6px;
            border: 1px solid var(--border);
            margin-bottom: 0.75rem;
            color: var(--text-muted);
        }
        .target-info strong { color: var(--cyan); }
        .status-active {
            color: var(--red);
            animation: blink 1s infinite;
        }
        @keyframes blink { 0%, 100% { opacity: 1; } 50% { opacity: 0.4; } }

        /* SSID TEXTAREA */
        .ssid-input {
            width: 100%;
            min-height: 120px;
            background: rgba(0,0,0,0.4);
            border: 1px solid var(--border);
            border-radius: 8px;
            padding: 0.75rem;
            color: var(--text);
            font-family: var(--font-mono);
            font-size: 0.75rem;
            resize: vertical;
            margin-bottom: 0.75rem;
            line-height: 1.6;
            outline: none;
            transition: border-color 0.25s;
        }
        .ssid-input:focus {
            border-color: var(--orange);
            box-shadow: 0 0 10px rgba(255,149,0,0.15);
        }
        .ssid-input::placeholder {
            color: var(--text-muted);
            opacity: 0.6;
        }
        .ssid-hint {
            font-size: 0.65rem;
            color: var(--text-muted);
            margin-bottom: 0.5rem;
            font-family: var(--font-mono);
        }

        /* CONSOLE */
        .console {
            background: #000;
            border: 1px solid var(--border);
            border-radius: 8px;
            padding: 1rem;
            height: 400px;
            overflow-y: auto;
            font-family: var(--font-mono);
            font-size: 0.72rem;
            line-height: 1.7;
            scroll-behavior: smooth;
        }
        .console::-webkit-scrollbar { width: 4px; }
        .console::-webkit-scrollbar-track { background: transparent; }
        .console::-webkit-scrollbar-thumb { background: var(--neon-dim); border-radius: 2px; }
        .log-line { white-space: pre-wrap; word-break: break-all; }
        .log-time { color: var(--text-muted); }
        .log-info { color: var(--cyan); }
        .log-warn { color: var(--orange); }
        .log-err { color: var(--red); }
        .log-ok { color: var(--neon); }

        /* SCANNING ANIMATION */
        .scanning-indicator {
            display: none;
            align-items: center;
            gap: 8px;
            font-family: var(--font-mono);
            font-size: 0.75rem;
            color: var(--cyan);
            margin: 0.75rem 0;
        }
        .scanning-indicator.active { display: flex; }
        .scan-dot {
            width: 8px; height: 8px;
            border-radius: 50%;
            background: var(--cyan);
            animation: scan-pulse 1.2s infinite;
        }
        @keyframes scan-pulse {
            0%, 100% { transform: scale(1); opacity: 1; }
            50% { transform: scale(1.8); opacity: 0.3; }
        }

        /* RESPONSIVE */
        @media (max-width: 600px) {
            .content { padding: 0.75rem; }
            .metric-grid { grid-template-columns: 1fr; }
            .scan-table { font-size: 0.65rem; }
            .scan-table th, .scan-table td { padding: 0.45rem 0.3rem; }
            .header { padding: 0.6rem 1rem; }
            .btn { padding: 0.5rem 0.85rem; font-size: 0.7rem; }
        }
    </style>
</head>
<body>

    <!-- HEADER -->
    <div class="header">
        <div class="logo">ECHO<span>NODE</span> <span style="color:#64748b;font-size:0.7rem;">// v2.0</span></div>
        <div class="status-badge">&#9679; AP ACTIVE</div>
    </div>

    <!-- TAB NAV -->
    <div class="tab-nav">
        <button class="tab-btn active" onclick="switchTab('dashboard')">
            <span class="tab-icon">&#9632;</span>DASHBOARD
        </button>
        <button class="tab-btn" onclick="switchTab('scanner')">
            <span class="tab-icon">&#9678;</span>SCANNER
        </button>
        <button class="tab-btn" onclick="switchTab('attacks')">
            <span class="tab-icon">&#9889;</span>ATTACKS
        </button>
        <button class="tab-btn" onclick="switchTab('console')">
            <span class="tab-icon">&gt;_</span>CONSOLE
        </button>
    </div>

    <div class="content">

        <!-- ==================== DASHBOARD TAB ==================== -->
        <div id="tab-dashboard" class="tab-panel active">
            <div class="card">
                <div class="card-title">System Metrics</div>
                <div class="metric-grid">
                    <div class="metric">
                        <div class="metric-label">Chip ID</div>
                        <div class="metric-value" id="v-chip">--</div>
                    </div>
                    <div class="metric">
                        <div class="metric-label">Free RAM</div>
                        <div class="metric-value" id="v-ram">--</div>
                    </div>
                    <div class="metric">
                        <div class="metric-label">Flash Size</div>
                        <div class="metric-value" id="v-flash">--</div>
                    </div>
                    <div class="metric">
                        <div class="metric-label">CPU Freq</div>
                        <div class="metric-value" id="v-cpu">--</div>
                    </div>
                    <div class="metric">
                        <div class="metric-label">Uptime</div>
                        <div class="metric-value" id="v-uptime">--</div>
                    </div>
                    <div class="metric">
                        <div class="metric-label">Connected Clients</div>
                        <div class="metric-value" id="v-clients">--</div>
                    </div>
                </div>
            </div>
        </div>

        <!-- ==================== SCANNER TAB ==================== -->
        <div id="tab-scanner" class="tab-panel">
            <div class="card">
                <div class="card-title">Wi-Fi Reconnaissance</div>
                <div class="btn-group">
                    <button class="btn btn-primary" id="btn-scan" onclick="startScan()">&#9678; SCAN NETWORKS</button>
                </div>
                <div class="scanning-indicator" id="scan-indicator">
                    <div class="scan-dot"></div>
                    Scanning nearby networks...
                </div>
                <div id="scan-results">
                    <p style="color:var(--text-muted);font-size:0.8rem;margin-top:1rem;font-family:var(--font-mono);">
                        Tarama baslatmak icin SCAN butonuna basin.
                    </p>
                </div>
            </div>
        </div>

        <!-- ==================== ATTACKS TAB ==================== -->
        <div id="tab-attacks" class="tab-panel">
            <!-- DEAUTH -->
            <div class="attack-section">
                <div class="attack-header">
                    <div class="attack-title" style="color:var(--red);">&#9889; Deauthentication</div>
                    <span id="deauth-status"></span>
                </div>
                <div class="attack-desc">
                    Hedef aga sahte deauthentication frame'leri gonderir. Bagli istemcilerin agdan dusurulmesini saglar. Scanner sekmesinden bir ag secin.
                </div>
                <div class="target-info" id="deauth-target">
                    Hedef: <strong>Secilmedi</strong> &mdash; Scanner sekmesinden bir ag secin.
                </div>
                <div class="btn-group">
                    <button class="btn btn-danger" id="btn-deauth" onclick="toggleDeauth()" disabled>&#9889; BASLA</button>
                    <button class="btn btn-primary" onclick="switchTab('scanner')" style="font-size:0.7rem;">AG SEC &rarr;</button>
                </div>
            </div>

            <!-- BEACON SPAM -->
            <div class="attack-section">
                <div class="attack-header">
                    <div class="attack-title" style="color:var(--orange);">&#9733; Beacon Flood</div>
                    <span id="beacon-status"></span>
                </div>
                <div class="attack-desc">
                    Asagiya yaymak istediginiz sahte ag isimlerini (SSID) her satira bir tane olacak sekilde yazin. Maksimum 20 adet.
                </div>
                <div class="ssid-hint">&#9654; Her satira bir SSID yazin (maks 32 karakter)</div>
                <textarea class="ssid-input" id="beacon-ssids" placeholder="Free WiFi&#10;FBI Surveillance Van&#10;Hack Me If You Can&#10;Virus Detected!&#10;Skynet Global">Free WiFi
FBI Surveillance Van
Hack Me If You Can
Virus Detected!
Skynet Global
Get Off My LAN
Trojan Network
Loading...</textarea>
                <div class="btn-group">
                    <button class="btn btn-warning" id="btn-beacon" onclick="toggleBeacon()">&#9733; BASLA</button>
                </div>
            </div>
        </div>

        <!-- ==================== CONSOLE TAB ==================== -->
        <div id="tab-console" class="tab-panel">
            <div class="card" style="padding:0.75rem;">
                <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:0.5rem;">
                    <div class="card-title" style="margin-bottom:0;">Live Console</div>
                    <button class="btn btn-primary" onclick="clearConsole()" style="font-size:0.65rem;padding:4px 10px;">TEMIZLE</button>
                </div>
                <div class="console" id="console-output">
                    <div class="log-line"><span class="log-ok">[SYS]</span> EchoNode v2.0 baslatildi.</div>
                    <div class="log-line"><span class="log-info">[AP]</span> EchoNode_Admin aktif.</div>
                </div>
            </div>
        </div>

    </div>

    <script>
        // ===== STATE =====
        let selectedTarget = null;
        let deauthActive = false;
        let beaconActive = false;

        // ===== TAB SWITCHING =====
        function switchTab(id) {
            document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
            document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
            document.getElementById('tab-' + id).classList.add('active');
            document.querySelectorAll('.tab-btn').forEach(b => {
                if (b.textContent.trim().toLowerCase().includes(id.substring(0,4))) b.classList.add('active');
            });
        }

        // ===== CONSOLE LOG =====
        function addLog(type, msg) {
            const con = document.getElementById('console-output');
            const now = new Date().toLocaleTimeString('tr-TR');
            const cls = {info:'log-info', warn:'log-warn', err:'log-err', ok:'log-ok'};
            const tags = {info:'INFO', warn:'WARN', err:'ERR', ok:'OK'};
            con.innerHTML += `<div class="log-line"><span class="log-time">${now}</span> <span class="${cls[type]||'log-info'}">[${tags[type]||'SYS'}]</span> ${msg}</div>`;
            con.scrollTop = con.scrollHeight;
        }
        function clearConsole() {
            document.getElementById('console-output').innerHTML = '';
            addLog('ok', 'Konsol temizlendi.');
        }

        // ===== DASHBOARD METRICS =====
        async function fetchStatus() {
            try {
                const r = await fetch('/api/status');
                const d = await r.json();
                document.getElementById('v-chip').innerText = d.chipId.toUpperCase();
                // freeHeap byte olarak geliyor, KB'ye cevir (ondalikli)
                let ramKB = (d.freeHeap / 1024).toFixed(1);
                document.getElementById('v-ram').innerText = ramKB + ' KB';
                // flashSize byte olarak geliyor, MB'ye cevir
                let flashMB = (d.flashSize / (1024 * 1024)).toFixed(0);
                document.getElementById('v-flash').innerText = flashMB + ' MB';
                document.getElementById('v-cpu').innerText = d.cpuFreq + ' MHz';
                document.getElementById('v-clients').innerText = d.clients;
                let u = d.uptime;
                let h = Math.floor(u/3600), m = Math.floor((u%3600)/60), s = u%60;
                document.getElementById('v-uptime').innerText = `${h}s ${m}d ${s}sn`;
            } catch(e) {}
        }

        // ===== SCANNER =====
        function signalBars(rssi) {
            let level = rssi > -50 ? 4 : rssi > -65 ? 3 : rssi > -78 ? 2 : 1;
            let bars = '';
            for (let i = 1; i <= 4; i++) {
                bars += `<div class="signal-bar${i <= level ? ' active' : ''}"></div>`;
            }
            return `<div class="signal">${bars}</div>`;
        }
        function encBadge(enc) {
            const map = { 'OPEN':'enc-open', 'WEP':'enc-wep', 'WPA':'enc-wpa', 'WPA2':'enc-wpa2', 'WPA/WPA2':'enc-wpa2', 'AUTO':'enc-wpa2' };
            return `<span class="enc-badge ${map[enc]||'enc-wpa2'}">${enc}</span>`;
        }

        async function startScan() {
            const btn = document.getElementById('btn-scan');
            const ind = document.getElementById('scan-indicator');
            btn.disabled = true;
            ind.classList.add('active');
            addLog('info', 'Wi-Fi taramasi baslatildi...');

            try {
                await fetch('/api/scan');

                let attempts = 0;
                const poll = setInterval(async () => {
                    attempts++;
                    try {
                        const r = await fetch('/api/scan/results');
                        const data = await r.json();

                        if (data.scanning) {
                            if (attempts > 30) {
                                clearInterval(poll);
                                ind.classList.remove('active');
                                btn.disabled = false;
                                addLog('err', 'Tarama zaman asimina ugradi.');
                            }
                            return;
                        }

                        clearInterval(poll);
                        ind.classList.remove('active');
                        btn.disabled = false;

                        const nets = Array.isArray(data) ? data : [];
                        addLog('ok', nets.length + ' ag bulundu.');

                        if (nets.length === 0) {
                            document.getElementById('scan-results').innerHTML = '<p style="color:var(--text-muted);font-size:0.8rem;margin-top:1rem;">Hicbir ag bulunamadi.</p>';
                            return;
                        }

                        let html = '<table class="scan-table"><thead><tr><th>SSID</th><th>BSSID</th><th>CH</th><th>RSSI</th><th>ENC</th><th>SIG</th></tr></thead><tbody>';
                        nets.forEach((n, i) => {
                            let ssid = n.ssid || '<hidden>';
                            html += `<tr onclick="selectTarget(${i})" id="net-${i}" data-bssid="${n.bssid}" data-ssid="${ssid}" data-ch="${n.channel}">`;
                            html += `<td>${ssid}</td>`;
                            html += `<td>${n.bssid}</td>`;
                            html += `<td>${n.channel}</td>`;
                            html += `<td>${n.rssi} dBm</td>`;
                            html += `<td>${encBadge(n.enc)}</td>`;
                            html += `<td>${signalBars(n.rssi)}</td>`;
                            html += `</tr>`;
                        });
                        html += '</tbody></table>';
                        document.getElementById('scan-results').innerHTML = html;
                    } catch(e) {
                        // Tarama sirasinda baglanti gecici olarak kopabilir.
                        // Hata durumunda iptal etme, sadece bekle ve tekrar dene.
                        if (attempts > 30) {
                            clearInterval(poll);
                            ind.classList.remove('active');
                            btn.disabled = false;
                            addLog('err', 'Tarama iptal oldu (Baglanti geri gelmedi).');
                        }
                    }
                }, 1000);
            } catch(e) {
                ind.classList.remove('active');
                btn.disabled = false;
                addLog('err', 'Tarama basarisiz: ' + e.message);
            }
        }

        function selectTarget(i) {
            document.querySelectorAll('.scan-table tr').forEach(r => r.classList.remove('selected'));
            const row = document.getElementById('net-' + i);
            row.classList.add('selected');
            selectedTarget = {
                bssid: row.dataset.bssid,
                ssid: row.dataset.ssid,
                channel: parseInt(row.dataset.ch)
            };
            document.getElementById('deauth-target').innerHTML = `Hedef: <strong>${selectedTarget.ssid}</strong> (${selectedTarget.bssid}) CH:${selectedTarget.channel}`;
            document.getElementById('btn-deauth').disabled = false;
            addLog('warn', 'Hedef secildi: ' + selectedTarget.ssid + ' [' + selectedTarget.bssid + ']');
        }

        // ===== DEAUTH =====
        async function toggleDeauth() {
            const btn = document.getElementById('btn-deauth');
            const st = document.getElementById('deauth-status');
            if (!deauthActive) {
                if (!selectedTarget) return;
                try {
                    const r = await fetch('/api/deauth', {
                        method: 'POST',
                        headers: {'Content-Type':'application/json'},
                        body: JSON.stringify(selectedTarget)
                    });
                    const d = await r.json();
                    if (d.ok) {
                        deauthActive = true;
                        btn.innerText = '&#9632; DURDUR';
                        btn.innerHTML = '&#9632; DURDUR';
                        st.innerHTML = '<span class="status-active">&#9679; AKTIF</span>';
                        addLog('err', 'DEAUTH saldirisi baslatildi: ' + selectedTarget.ssid);
                    }
                } catch(e) { addLog('err', 'Deauth basarisiz: ' + e.message); }
            } else {
                try {
                    await fetch('/api/deauth/stop');
                    deauthActive = false;
                    btn.innerHTML = '&#9889; BASLA';
                    st.innerHTML = '';
                    addLog('ok', 'Deauth durduruldu.');
                } catch(e) { addLog('err', 'Durdurma hatasi: ' + e.message); }
            }
        }

        // ===== BEACON SPAM =====
        async function toggleBeacon() {
            const btn = document.getElementById('btn-beacon');
            const st = document.getElementById('beacon-status');
            const ta = document.getElementById('beacon-ssids');
            if (!beaconActive) {
                let ssidText = ta.value.trim();
                if (!ssidText) {
                    addLog('err', 'Lutfen en az bir SSID girin!');
                    return;
                }
                try {
                    const r = await fetch('/api/beacon', {
                        method: 'POST',
                        headers: {'Content-Type':'application/json'},
                        body: JSON.stringify({ ssids: ssidText })
                    });
                    const d = await r.json();
                    if (d.ok) {
                        beaconActive = true;
                        ta.disabled = true;
                        btn.innerHTML = '&#9632; DURDUR';
                        btn.classList.remove('btn-warning');
                        btn.classList.add('btn-danger');
                        st.innerHTML = '<span class="status-active">&#9679; AKTIF</span>';
                        addLog('warn', 'Beacon flood baslatildi! ' + d.count + ' SSID yayinlaniyor.');
                    } else {
                        addLog('err', 'Beacon basarisiz: ' + (d.err || 'bilinmeyen hata'));
                    }
                } catch(e) { addLog('err', 'Beacon basarisiz: ' + e.message); }
            } else {
                try {
                    await fetch('/api/beacon/stop');
                    beaconActive = false;
                    ta.disabled = false;
                    btn.innerHTML = '&#9733; BASLA';
                    btn.classList.remove('btn-danger');
                    btn.classList.add('btn-warning');
                    st.innerHTML = '';
                    addLog('ok', 'Beacon flood durduruldu.');
                } catch(e) { addLog('err', 'Durdurma hatasi: ' + e.message); }
            }
        }

        // ===== INIT =====
        fetchStatus();
        setInterval(fetchStatus, 4000);
    </script>
</body>
</html>
)=====";

#endif
