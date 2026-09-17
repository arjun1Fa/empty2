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

})();