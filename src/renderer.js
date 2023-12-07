const { configureStore } = require("@reduxjs/toolkit");
const React = require("react");
const ReactDOM = require("react-dom");
const { Provider } = require("react-redux");
const NotebookApp = require("@nteract/notebook-app-component");
const { createEpicMiddleware } = require("redux-observable");
const { contents } = require("rx-jupyter");

const { reducer, initialState, contentRef } = require("./json-reducer");

const epicMiddleware = createEpicMiddleware({ dependencies: contents.JupyterContentProvider });

const store = configureStore({
  reducer,
  preloadedState: initialState,
  middleware: [epicMiddleware],
});

const App = () => React.createElement(
  Provider,
  { store: store },
  React.createElement(NotebookApp, { contentRef: contentRef })
);

const rootElement = document.getElementById("root");
ReactDOM.render(React.createElement(App), rootElement);
