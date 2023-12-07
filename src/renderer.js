/**
 * @file This file contains the code for rendering the React application and initializing the Jupyter kernel connection.
 */

const React = require('react');
const ReactDOM = require('react-dom');
const { Provider } = require('react-redux');
const { combineReducers } = require('redux');
const { NotebookApp } = require('@nteract/notebook-app-component');
const { kernels, notebooks, transforms } = require('@nteract/core');
const { configureStore } = require('@reduxjs/toolkit');

/**
 * Combines the reducers for kernels, notebooks, and transforms into a single root reducer.
 * @type {Function}
 */
const rootReducer = combineReducers({
  kernels,
  notebooks,
  transforms
});

/**
 * The initial state for the core reducer.
 * @type {Object}
 */
const initialCoreState = {
  kernels: kernels.makeStateRecord(),
  notebooks: notebooks.makeStateRecord()
};

/**
 * Creates the Redux store with the root reducer and initial state.
 * @type {Object}
 */

const store = configureStore({
  reducer: rootReducer,
  preloadedState: { core: initialCoreState }
});

// initialize the Jupyter kernel connection
store.dispatch(kernels.launchKernelByName({ kernelSpecName: 'python3', cwd: '.' }));

/**
 * The main React component that wraps the application with the Redux Provider.
 * @returns {JSX.Element} The rendered React component.
 */
const App = () => (
  React.createElement(Provider, { store },
    React.createElement(NotebookApp)
  )
);

ReactDOM.render(React.createElement(App), document.getElementById('root'));
