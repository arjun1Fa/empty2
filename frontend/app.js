/**
 * ============================================================================
 * DebugCalc - Frontend Application Logic
 * ============================================================================
 * 
 * NOTE FOR STUDENTS:
 * This frontend code is completely bug-free and fully functional.
 * It is responsible for:
 *   1. Monitoring backend health via GET http://localhost:8080/health
 *   2. Capturing user inputs (operand a, operand b, and operation)
 *   3. Calling GET http://localhost:8080/api/calculate?op=...&a=...&b=...
 *   4. Rendering results, expressions, errors, and inspector logs
 * 
 * Your debugging tasks are in the C backend (backend/main.c).
 * ============================================================================
 */

(function () {
  'use strict';

  // Configuration
  const API_BASE_URL = 'http://localhost:8080';
  const HEALTH_CHECK_INTERVAL_MS = 4000;

  // DOM Elements - Status
  const statusDot = document.getElementById('status-dot');
  const statusText = document.getElementById('status-text');
  const statusSubtext = document.getElementById('status-subtext');

  // DOM Elements - Inputs & Swapper
  const inputA = document.getElementById('input-a');
  const inputB = document.getElementById('input-b');
  const btnClearA = document.getElementById('btn-clear-a');
  const btnClearB = document.getElementById('btn-clear-b');
  const btnSwap = document.getElementById('btn-swap');

  // DOM Elements - Operations
  const opButtons = document.querySelectorAll('.op-btn');
  const screenOpBadge = document.getElementById('screen-op-badge');

  // DOM Elements - Displays
  const displayExpression = document.getElementById('display-expression');
  const displayResult = document.getElementById('display-result');

  // DOM Elements - Actions
  const btnClearAll = document.getElementById('btn-clear-all');
  const btnCompute = document.getElementById('btn-compute');
  const btnComputeText = document.getElementById('btn-compute-text');
  const computeSpinner = document.getElementById('compute-spinner');

  // DOM Elements - Error Banner
  const errorBanner = document.getElementById('error-banner');
  const errorMessage = document.getElementById('error-message');
  const btnErrorDismiss = document.getElementById('btn-error-dismiss');

  // DOM Elements - Inspector
  const inspectorUrl = document.getElementById('inspector-url');
  const inspectorResponse = document.getElementById('inspector-response');
  const inspectorHttpStatus = document.getElementById('inspector-http-status');

  // Application State
  let currentOperation = 'add'; // 'add', 'subtract', 'multiply', 'divide'
  let isBackendOnline = false;
  let isCalculating = false;

  const OP_SYMBOLS = {
    add: '+',
    subtract: '−',
    multiply: '×',
    divide: '÷'
  };

  /**
   * Set backend status indicator
   */
  function updateBackendStatus(state, message) {
    statusDot.className = 'status-dot ' + state;
    statusText.textContent = message;

    if (state === 'online') {
      isBackendOnline = true;
      statusSubtext.textContent = 'Connected (Port 8080)';
    } else if (state === 'offline') {
      isBackendOnline = false;
      statusSubtext.textContent = 'Server unreachable';
    } else {
      statusSubtext.textContent = 'Connecting...';
    }
  }

  /**
   * Check backend health endpoint: GET /health
   */
  async function checkBackendHealth() {
    try {
      const controller = new AbortController();
      const timeoutId = setTimeout(() => controller.abort(), 2500);

      const response = await fetch(`${API_BASE_URL}/health`, {
        method: 'GET',
        signal: controller.signal,
        headers: { 'Accept': 'application/json' }
      });
      clearTimeout(timeoutId);

      if (response.ok) {
        const data = await response.json();
        if (data.status === 'ok') {
          updateBackendStatus('online', 'Backend Online');
          return;
        }
      }
      updateBackendStatus('offline', 'Backend Issue');
    } catch (err) {
      updateBackendStatus('offline', 'Backend Offline');
    }
  }

  /**
   * Display error alert banner
   */
  function showError(msg) {
    errorMessage.textContent = msg || 'An unknown error occurred.';
    errorBanner.classList.remove('hidden');
  }

  /**
   * Hide error alert banner
   */
  function clearError() {
    errorBanner.classList.add('hidden');
    errorMessage.textContent = '';
  }

  /**
   * Set the active mathematical operation
   */
  function selectOperation(op) {
    currentOperation = op;

    opButtons.forEach(btn => {
      if (btn.dataset.op === op) {
        btn.classList.add('active');
      } else {
        btn.classList.remove('active');
      }
    });

    const symbol = OP_SYMBOLS[op] || '+';
    screenOpBadge.textContent = `${op.toUpperCase()} (${symbol})`;
    updateExpressionPreview();
  }

  /**
   * Live preview of expression on the screen
   */
  function updateExpressionPreview() {
    const valA = inputA.value.trim();
    const valB = inputB.value.trim();
    const symbol = OP_SYMBOLS[currentOperation] || '+';

    if (valA === '' && valB === '') {
      displayExpression.innerHTML = '<span>0</span>';
    } else {
      const displayA = valA !== '' ? valA : '?';
      const displayB = valB !== '' ? valB : '?';
      displayExpression.innerHTML = `<span>${displayA} ${symbol} ${displayB} =</span>`;
    }
  }

  /**
   * Clear all inputs and displays
   */
  function clearAll() {
    inputA.value = '';
    inputB.value = '';
    displayExpression.innerHTML = '<span>0</span>';
    displayResult.innerHTML = '<span>0</span>';
    clearError();
    inspectorHttpStatus.textContent = 'Idle';
    inspectorHttpStatus.className = 'inspector-status-badge';
    inspectorUrl.textContent = 'Waiting for calculation...';
    inspectorResponse.textContent = '{}';
    selectOperation('add');
    inputA.focus();
  }

  /**
   * Swap operand A and operand B
   */
  function swapOperands() {
    const temp = inputA.value;
    inputA.value = inputB.value;
    inputB.value = temp;
    updateExpressionPreview();
  }

  /**
   * Execute calculation via C backend API
   */
  async function performCalculation() {
    clearError();

    const rawA = inputA.value.trim();
    const rawB = inputB.value.trim();

    if (rawA === '' || rawB === '') {
      showError('Please enter both First Number (a) and Second Number (b).');
      return;
    }

    if (isCalculating) return;

    // Construct API URL
    const params = new URLSearchParams({
      op: currentOperation,
      a: rawA,
      b: rawB
    });
    const requestUrl = `${API_BASE_URL}/api/calculate?${params.toString()}`;

    // Update inspector
    inspectorUrl.textContent = `GET ${requestUrl}`;
    inspectorHttpStatus.textContent = 'Sending...';
    inspectorHttpStatus.className = 'inspector-status-badge';
    inspectorResponse.textContent = 'Awaiting response...';

    // UI Loading state
    isCalculating = true;
    btnCompute.disabled = true;
    btnComputeText.textContent = 'Calculating...';
    computeSpinner.classList.remove('hidden');

    const symbol = OP_SYMBOLS[currentOperation] || '+';
    displayExpression.innerHTML = `<span>${rawA} ${symbol} ${rawB} =</span>`;

    try {
      const controller = new AbortController();
      const timeoutId = setTimeout(() => controller.abort(), 6000);

      const response = await fetch(requestUrl, {
        method: 'GET',
        signal: controller.signal,
        headers: { 'Accept': 'application/json' }
      });
      clearTimeout(timeoutId);

      const statusCode = response.status;
      const statusTextStr = response.statusText || (response.ok ? 'OK' : 'Error');

      // Update inspector status badge
      inspectorHttpStatus.textContent = `${statusCode} ${statusTextStr}`;
      inspectorHttpStatus.className = 'inspector-status-badge ' +
        (statusCode === 200 ? 'status-200' : 'status-error');

      let responseData = null;
      const responseText = await response.text();

      try {
        responseData = JSON.parse(responseText);
        inspectorResponse.textContent = JSON.stringify(responseData, null, 2);
      } catch (parseErr) {
        inspectorResponse.textContent = responseText || `(Empty or non-JSON response)`;
      }

      // Check if backend returned an error
      if (responseData && responseData.error !== undefined) {
        showError(`Backend error: ${responseData.error}`);
        displayResult.innerHTML = `<span style="color: var(--danger);">Error</span>`;
        return;
      }

      if (!response.ok) {
        const errorDetail = (responseData && responseData.error) ? responseData.error : `HTTP ${statusCode}`;
        showError(`Backend request failed with status: ${errorDetail}`);
        displayResult.innerHTML = `<span style="color: var(--danger);">Error</span>`;
        return;
      }

      if (responseData && responseData.result !== undefined) {
        displayResult.innerHTML = `<span>${responseData.result}</span>`;
      } else {
        showError('Invalid backend response: missing "result" field.');
        displayResult.innerHTML = `<span style="color: var(--warning);">Invalid</span>`;
      }

    } catch (networkError) {
      const isTimeout = networkError.name === 'AbortError';
      const msg = isTimeout
        ? 'Request timed out after 6 seconds. Is the C backend running on port 8080?'
        : 'Failed to connect to backend at http://localhost:8080. Start backend/main.c first.';
      
      showError(msg);
      displayResult.innerHTML = `<span style="color: var(--danger);">Offline</span>`;
      inspectorHttpStatus.textContent = 'Network Error';
      inspectorHttpStatus.className = 'inspector-status-badge status-error';
      inspectorResponse.textContent = JSON.stringify({ error: msg }, null, 2);
      updateBackendStatus('offline', 'Backend Offline');
    } finally {
      isCalculating = false;
      btnCompute.disabled = false;
      btnComputeText.textContent = 'Calculate Result';
      computeSpinner.classList.add('hidden');
    }
  }

})();