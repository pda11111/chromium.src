// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var allTests = [
  // Basic query from root node.
  function testQuerySelector() {
    var cancelButton = rootNode.lastChild().lastChild();
    function assertCorrectResult(queryResult) {
      assertEq(queryResult, cancelButton);
      chrome.test.succeed();
    }
    rootNode.querySelector('body > button:nth-of-type(2)',
                           assertCorrectResult);
  },

  function testQuerySelectorNoMatch() {
    function assertCorrectResult(queryResult) {
      assertEq(null, queryResult);
      chrome.test.succeed();
    }
    rootNode.querySelector('#nonexistent',
                           assertCorrectResult);
  },

  // Demonstrates that a query from a non-root element queries inside that
  // element.
  function testQuerySelectorFromMain() {
    var main = rootNode.children()[1];
    // paragraph inside "main" element - not the first <p> on the page
    var p = main.firstChild();
    function assertCorrectResult(queryResult) {
      assertEq(queryResult, p);
      chrome.test.succeed();
    }
    main.querySelector('p', assertCorrectResult);
  },

  // Demonstrates that a query for an element which is ignored for accessibility
  // returns its nearest ancestor.
  function testQuerySelectorForSpanInsideButtonReturnsButton() {
    var okButton = rootNode.lastChild().firstChild();
    function assertCorrectResult(queryResult) {
      assertEq(queryResult, okButton);
      chrome.test.succeed();
    }
    rootNode.querySelector('#span-in-button', assertCorrectResult);
  },

  // Demonstrates that querying from an anonymous node may have unexpected
  // results.
  function testQuerySelectorFromAnonymousGroup() {
    var h1 = rootNode.firstChild().firstChild();
    var group = rootNode.lastChild();
    function assertCorrectResult(queryResult) {
      assertEq(h1, queryResult);
      chrome.test.succeed();
    }
    group.querySelector('h1', assertCorrectResult);
  },

  function testQuerySelectorFromRemovedNode() {
    var group = rootNode.firstChild();
    function assertCorrectResult(queryResult) {
      assertEq(null, queryResult);
      var errorMsg =
          'querySelector sent on node which is no longer in the tree.';
      assertEq(errorMsg, chrome.extension.lastError.message);
      assertEq(errorMsg, chrome.runtime.lastError.message);

      chrome.test.succeed();
    }
    function afterRemoveChild() {
      group.querySelector('h1', assertCorrectResult);
    }
    chrome.tabs.executeScript(
        { code: 'document.body.removeChild(document.body.firstElementChild)' },
        afterRemoveChild);
  }
];

setUpAndRunTests(allTests, 'complex.html');
