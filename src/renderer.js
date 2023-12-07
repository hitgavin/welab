const { configureStore } = require("@reduxjs/toolkit");
const React = require("react");
const ReactDOM = require("react-dom");
const { Provider } = require("react-redux");
const NotebookApp = require("@nteract/notebook-app-component");
const { makeEntitiesRecord, makeContentsRecord, makeStateRecord } = require("@nteract/core");
const { reducers, createContentRef, createKernelspecsRef, makeNotebookContentRecord } = require("@nteract/core");
const { createEpicMiddleware } = require("redux-observable");
const { contents } = require("rx-jupyter");

const epicMiddleware = createEpicMiddleware({ dependencies: contents.JupyterContentProvider });

const contentRef = createContentRef();
const kernelspecsRef = createKernelspecsRef();

const initialRefs = Immutable.Map().set(
  contentRef,
  makeNotebookContentRecord()
);

const initialState = {
  core: makeStateRecord({
    currentKernelspecsRef: kernelspecsRef,
    entities: makeEntitiesRecord({
      contents: makeContentsRecord({
        byRef: initialRefs
      })
    })
  })
}

const store = configureStore({
  reducer: reducers,
  preloadedState: initialState,
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware({ serializableCheck: false }).concat(epicMiddleware)
});

const App = () => React.createElement(
  Provider,
  { store: store },
  React.createElement(NotebookApp, { contentRef: contentRef })
);

const rootElement = document.getElementById("root");
ReactDOM.render(App, rootElement);
