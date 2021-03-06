window.Common = (function() {
  "use strict";

  var DefaultResourceURI = "/movies",
      DefaultCollectionHolderSelector = "#movies";

  // Non-standard currying arguments method.
  if (!Function.prototype.curry) {
    Function.prototype.curry = function() {
      if (arguments.length < 1) {
          return this;
      }

      var method = this,
          args = [].slice.call(arguments);

      return function() {
          return method.apply(this, args.concat([].slice.call(arguments)));
      };
    };
  }

  // Because all JavaScript projects have jQuery ;)
  function $(selector) {
    var results = document.querySelectorAll(selector);

    return results.length == 1 ? results[0] : results;
  }

  function splat(element) {
    var isNodeList = Object.prototype.toString.call(element).indexOf("NodeList") !== -1,
        isArray = Array.isArray(element);

    if (isNodeList) {
      element = Array.prototype.slice.call(element);
    }

    return isNodeList || isArray ? element : [ element ];
  }

  // Hide elements
  function hideElement(element) {
    element.classList.add("hidden");
  }

  // Methods related with overlay.
  function hideOverlay() {
    $(".overlay").classList.add("hidden");
  }

  function showOverlay() {
    $(".overlay").classList.remove("hidden");
  }

  // XHR methods.
  function xhr(method, path, callback, args) {
    var request = new XMLHttpRequest();

    request.open(method, path, true);

    if (method.toUpperCase() === "POST") {
      request.setRequestHeader("Content-Type", "application/json");
    }

    request.addEventListener("load", callback || hideOverlay);
    request.send(JSON.stringify(args));

    showOverlay();
  }

  function xhrGet(path, callback) {
    xhr("GET", path, callback);
  }

  function xhrPost(path, args, callback) {
    xhr("POST", path, callback, args);
  }

  // Button helpers.
  function toggleButton(selector, value) {
    var button = $(selector);

    if (!!value) {
      button.removeAttribute("disabled");
    } else {
      button.setAttribute("disabled", "disabled");
    }
  }

  function buildElementList(callback, destination) {
    var response = JSON.parse(this.responseText),
        elements = $(destination);

    response.forEach(function(element) {
      var option = document.createElement("option");

      option.setAttribute("value", element.value);
      option.innerText = element.name;

      elements.appendChild(option);
    });

    if (typeof(callback) === "function") {
      callback();
    }
  }

  // Ger element to select helpers.
  function getCollection(callback, url, destination) {
    url = url || DefaultResourceURI;
    destination = destination || DefaultCollectionHolderSelector;

    xhrGet(url, buildElementList.curry(callback, destination));
  }

  function getOptionText(from) {
    var select;

    from = from || DefaultCollectionHolderSelector;
    select = $(from);

    return select.options[select.selectedIndex].innerText;
  }

  function getOptionValue(from) {
    var select;

    from = from || DefaultCollectionHolderSelector;
    select = $(from);

    return parseInt(select.options[select.selectedIndex].value, 10);
  }

  function changeVideo(name) {
    if (!!name) {
      $("#player").setAttribute("src", "/videos-converted/%webm".replace("%webm", name));
    }
  }

  return {
    $: $,

    splat: splat,

    xhrGet: xhrGet,
    xhrPost: xhrPost,

    getCollection: getCollection,
    getOptionText: getOptionText,
    getOptionValue: getOptionValue,

    changeVideo: changeVideo,

    toggleButton: toggleButton,
    hideElement: hideElement,

    hideOverlay: hideOverlay
  };
} ());