import { configureStore } from "@reduxjs/toolkit";
const React = require("react");
const ReactDOM = require("react-dom");
const { Provider } = require("react-redux");
const NotebookApp = require("@nteract/notebook-app-component");
const { applyMiddleware, createStore } = require("redux");
const { createContentRef } = require("@nteract/core");
const { reducer, initialState, contentRef } = require("./json-reducer");
const { createEpicMiddleware } = require("redux-observable");
const { contents } = require("rx-jupyter");

const epicMiddleware = createEpicMiddleware({ dependencies: contents.JupyterContentProvider });

const store = configureStore({
  reducer,
  preloadedState: initialState,
  middleware: [epicMiddleware],
});

const App = () => (
  <Provider store={store}>
    <NotebookApp contentRef={contentRef} />
  </Provider>
);

const rootElement = document.getElementById("root");
ReactDOM.render(<App />, rootElement);