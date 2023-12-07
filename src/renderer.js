const { configureStore } = require("@reduxjs/toolkit");
const React = require("react");
const ReactDOM = require("react-dom");
const { Provider } = require("react-redux");
const NotebookApp = require("@nteract/notebook-app-component");
const { reducers, createContentRef } = require("@nteract/core");
const { createEpicMiddleware } = require("redux-observable");
const { contents } = require("rx-jupyter");

const epicMiddleware = createEpicMiddleware({ dependencies: contents.JupyterContentProvider });

const initialState = {
  core: {
    entities: {
      contents: {},
      kernels: {},
      kernelspecs: {}
    }
  },
  config: {
    // 这里可以放置一些全局配置选项
  },
};

const store = configureStore({
  reducer: reducers,
  preloadedState: initialState,
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware({ serializableCheck: false }).concat(epicMiddleware)
});

const contentRef = createContentRef();

const App = () => React.createElement(
  Provider,
  { store: store },
  React.createElement(NotebookApp, { contentRef: contentRef })
);

const rootElement = document.getElementById("root");
ReactDOM.render(App, rootElement);
