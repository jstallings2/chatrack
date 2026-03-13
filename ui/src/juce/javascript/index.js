/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

import "./check_native_interop.js";

class PromiseHandler {
  constructor() {
    this.lastPromiseId = 0;
    this.promises = new Map();

    window.__JUCE__.backend.addEventListener(
      "__juce__complete",
      ({ promiseId, result }) => {
        if (this.promises.has(promiseId)) {
          this.promises.get(promiseId).resolve(result);
          this.promises.delete(promiseId);
        }
      }
    );
  }

  createPromise() {
    const promiseId = this.lastPromiseId++;
    const result = new Promise((resolve, reject) => {
      this.promises.set(promiseId, { resolve: resolve, reject: reject });
    });
    return [promiseId, result];
  }
}

const promiseHandler = new PromiseHandler();

/**
 * Returns a function object that calls a function registered on the JUCE backend and forwards all
 * parameters to it.
 *
 * The provided name should be the same as the name argument passed to
 * WebBrowserComponent::Options.withNativeFunction() on the backend.
 *
 * @param {String} name
 */
function getNativeFunction(name) {
  if (!window.__JUCE__.initialisationData.__juce__functions.includes(name))
    console.warn(
      `Creating native function binding for '${name}', which is unknown to the backend`
    );

  const f = function () {
    const [promiseId, result] = promiseHandler.createPromise();

    window.__JUCE__.backend.emitEvent("__juce__invoke", {
      name: name,
      params: Array.prototype.slice.call(arguments),
      resultId: promiseId,
    });

    return result;
  };

  return f;
}

/**
 * Add an event listener for events emitted from the C++ backend.
 * @param {String} eventId - The event identifier
 * @param {Function} callback - The callback function
 * @returns {Array} - A handle that can be used to remove the listener
 */
function addEventListener(eventId, callback) {
  return window.__JUCE__.backend.addEventListener(eventId, callback);
}

/**
 * Remove an event listener.
 * @param {Array} handle - The handle returned from addEventListener
 */
function removeEventListener(handle) {
  window.__JUCE__.backend.removeEventListener(handle);
}

/**
 * Appends a platform-specific prefix to the path to ensure that a request sent to this address will
 * be received by the backend's ResourceProvider.
 * @param {String} path
 */
function getBackendResourceAddress(path) {
  const platform =
    window.__JUCE__.initialisationData.__juce__platform.length > 0
      ? window.__JUCE__.initialisationData.__juce__platform[0]
      : "";

  if (platform == "windows" || platform == "android")
    return "https://juce.backend/" + path;

  if (platform == "macos" || platform == "ios" || platform == "linux")
    return "juce://juce.backend/" + path;

  console.warn(
    "getBackendResourceAddress() called, but no JUCE native backend is detected."
  );
  return path;
}

export {
  getNativeFunction,
  addEventListener,
  removeEventListener,
  getBackendResourceAddress,
};
